/*
** EPITECH PROJECT, 2025
** maverik
** File description:
** Software
*/

#include "vk/Software.hpp"

/*
** Construct a new Software object.
**
** This constructor is used to create a new Software object.
** It initializes the application name, application version, engine name and engine version.
**
** The default values are:
** - Application name `_appName`: "Hello, World !"
** - Application version `_appVersion`: 1.0.0
** - Engine name `_engineName`: "Maverik"
** - Engine version `_engine`: 1.0.0
*/
maverik::vk::Software::Software()
{
    this->_appName = "Hello, World !";
    this->_appVersion = new Version(1, 0, 0);
    this->_engineName = "Maverik";
    this->_engineVersion = new Version(1, 0, 0);
    this->_graphicalContext = std::make_shared<maverik::vk::GraphicalContext>();
}

/*
** Construct a new Software object.
**
** This constructor is used to create a new Software object.
** It initializes the application name, application version, engine name and engine version.
**
** @param appName the application name
** @param appVersion the application version
** @param engineName the engine name
** @param engineVersion the engine version
**
*/
maverik::vk::Software::Software(const std::string &appName, const Version &appVersion, const std::string &engineName, const Version &engineVersion)
{
    this->_appName = appName;
    this->_appVersion = new Version(appVersion);
    this->_engineName = engineName;
    this->_engineVersion = new Version(engineVersion);
    this->_graphicalContext = std::make_shared<maverik::vk::GraphicalContext>(appName, appVersion, engineName, engineVersion);
}

/*
** Copy constructor for the Software class.
**
** This constructor is used to create a new Software object from an existing one.
** It initializes the application name, application version, engine name and engine version.
**
** @param other the Software object to copy
**
*/
maverik::vk::Software::Software(const Software &other)
{
    this->_appName = other._appName;
    this->_appVersion = new Version(*other._appVersion);
    this->_engineName = other._engineName;
    this->_engineVersion = new Version(*other._engineVersion);
    this->_graphicalContext = std::make_shared<maverik::vk::GraphicalContext>(*other._graphicalContext);
}

maverik::vk::Software::~Software()
{
}
