#include "Channel.hpp"

Channel::Channel(std::string const name, std::string const topic, std::string const mode, User &op): _name(name), _topic(topic), _mode(mode)
{
    std::pair<int, User *> new_pair(op.get_fd(), &op);

    _users.insert(new_pair);
    _users_mode[op.get_fd()] = "o";
    return;
}
Channel::Channel(std::string const &name, std::string const &topic): _name(name), _topic(topic) {return;}
Channel::~Channel() {}

//setter
void    Channel::set_name(std::string const name) {_name = name;}
void    Channel::set_topic(std::string const topic) {_topic = topic;}
void    Channel::set_user_max(std::string const user_max) {_user_max = user_max;}
void    Channel::set_pass(std::string const pass) {_pass = pass;}
bool    Channel::set_mode(std::string const mode)
{
    for (std::string::const_iterator it = mode.begin(); it != mode.end(); it++)
    {
        if (_mode.find(*it) == std::string::npos)
        {
            _mode += *it;
            return true;
        }
    }
    return false;
}
bool    Channel::unset_mode(std::string const mode)
{
    for (std::string::const_iterator it = mode.begin(); it != mode.end(); it++)
    {
        if (_mode.find(*it) != std::string::npos)
        {
            _mode.erase(_mode.find(*it), mode.length());
            return true;
        }
    }
    return false;
}

//users_mode take the value mode
bool    Channel::set_user_mode(User user, char const mode)
{
    if (_users_mode.find(user.get_fd()) != _users_mode.end())
    {
            if (_users_mode[user.get_fd()].find(mode) != std::string::npos)
                return false;
        _users_mode[user.get_fd()] += mode;
        return true;
    }
    return false;
}

bool    Channel::unset_user_mode(User user, char const mode)
{
    if (_users.find(user.get_fd()) != _users.end())
    {
            if (_users_mode[user.get_fd()].find(mode) != std::string::npos)
            {
                _users_mode[user.get_fd()].erase(_users_mode[user.get_fd()].find(mode), 1);
                return true;
            }
    }
    return false;
}

//getter
std::string     Channel::get_name() const {return _name;}
std::string     Channel::get_topic() const {return _topic;}
std::string     Channel::get_mode() const {return _mode;}
std::string     Channel::get_user_max() const {return _user_max;}
std::string     Channel::get_pass() const {return _pass;}
//return le mode associer au nickname ou une chaine vide
std::string     Channel::get_user_mode(User user)
{
    if (_users.find(user.get_fd()) != _users.end())
        return _users_mode[user.get_fd()] + user.get_mode();
    return "";
}

std::map<int, User *>    Channel::get_users() const {return _users;}

//add/remove user
bool    Channel::add_user(User &user)
{
    if (_users.find(user.get_fd()) != _users.end())
        return false;   // error: peut-etre retour erreur si nickname identique ou exception
    /*if (_mode_enable("i"))
        if (!_is_invited(user.get_fd()))
            return false; //error: not invited
    if (_mode_enable("l") && _users.size() < _user_max)
        return false; //error nb max_user atteint*/
    std::pair<int, User *> new_pair(user.get_fd(), &user);
    _users.insert(new_pair);
    _users_mode[user.get_fd()] = "";
    return true;
}

void    Channel::remove_user(User user)
{
    std::map<int, User *>::iterator find = _users.find(user.get_fd());

    if (find != _users.end())
    {
        _users.erase(find);
        _users_mode.erase(_users_mode.find(user.get_fd()));
    }
    return;
}

//add/remove invite
std::map<int, User *>    Channel::get_invited() const {return _invited;}

void    Channel::add_invited(User &user)
{
    if (_invited.find(user.get_fd()) != _invited.end())
        return;    //peut-etre retour erreur si fd identique ou exception
    std::pair<int, User *> new_pair(user.get_fd(), &user);
    _invited.insert(new_pair);
    return;
}

void    Channel::remove_invited(User user)
{
    std::map<int, User *>::iterator find = _invited.find(user.get_fd());

    if (find != _invited.end())
        _invited.erase(find);
    return;
}

std::ostream&	operator<<(std::ostream& os, const Channel& chan)
{
	std::map<int, User *>::const_iterator	ite;
	
	os << "NAME = " << chan._name << std::endl;
	os << "TOPIC = " << chan._topic << std::endl;
	os << "MODE = " << chan._mode << std::endl;
	os << "USERS = " << std::endl << std::endl;
	for (ite = chan.get_users().begin(); ite != chan._users.end(); ite++)
		os << ite->second;
	return (os);
}

//broadcast
void    Channel::broadcast(Command cmd, std::string msg)
{
    for (std::map<int, User *>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        it->second->send_msg(cmd, "", msg);
    }
}

//banmask
std::vector<std::string>    Channel::get_banmask() const {return _banmask;}
bool                        Channel::add_banmask(std::string banmask)
{
    for (std::vector<std::string>::iterator it = _banmask.begin(); it != _banmask.end(); it++)
    {
        if (*it == banmask)
            return false;
    }
    _banmask.push_back(banmask);
    return true;
}

bool                        Channel::remove_banmask(std::string banmask)
{
    for (std::vector<std::string>::iterator it = _banmask.begin(); it != _banmask.end(); it++)
    {
        if (*it == banmask)
        {
            _banmask.erase(it);
            return true;
        }
    }
    return false;
}

bool                        Channel::is_ban(User &user) const
{
    for (std::vector<std::string>::const_iterator it = _banmask.begin(); it != _banmask.end(); it++)
        if (_str_is_egal(user.get_prefix().c_str(), it->c_str(), user.get_prefix().length(), it->length()))
            return true;
    return false;
}

//private
//return true si les 2 chaines corresponde en comptant les wildcard.
bool    Channel::_str_is_egal(const char *str, const char *pattern, int m, int n) const
{
    std::vector<bool> prev(m + 1, false), curr(m + 1, false);
 
    prev[0] = true;
 
    for (int i = 1; i <= n; i++)
    {
        bool flag = true;
        for (int ii = 1; ii < i; ii++) {
            if (pattern[ii - 1] != '*') {
                flag = false;
                break;
            }
        }
        curr[0] = flag; 
        for (int j = 1; j <= m; j++) {
            if (pattern[i - 1] == '*')
                curr[j] = curr[j - 1] || prev[j];
            else if (str[j - 1] == pattern[i - 1])
                curr[j] = prev[j - 1];
            else
                curr[j] = false;
        }
        prev = curr;
    }
    return prev[m];
}

bool    Channel::_mode_enable(char mode) const
{
    //return true si le mode est enable
    if (_mode.find(mode) != std::string::npos)
        return true;
    return false;
}

bool    Channel::is_invited(User user) const
{
    if (_invited.find(user.get_fd()) != _invited.end())
        return true;
    return false;
}