#include "woap_configuration_manager.h"
#include "woap_main.h"

using namespace WOAP;

SINGLETON_IMPLEMENTATION(ConfigurationManager)

ConfigurationManager::ConfigurationManager()
{
	String err;
	if (!setConfiguration(getDefaultConfiguration(), err))
		jassertfalse;
}

ConfigurationManager::~ConfigurationManager()
{

}

ConfigurationManager::Configuration ConfigurationManager::getDefaultConfiguration() const
{
	Configuration c;

	c.loadPreviousSession = true;
	c.sessionStoreIntervalSecs = 10;

	c.anonymousLoginEnabled = true;
	c.adminPassword = "admin";

	c.WLANEnabled = false;
	c.WLANName = "WOAP";
	c.WLANPassword = "adminwoap";

	return c;
}

void ConfigurationManager::setConfiguration(const Configuration& c)
{
	String err;
	setConfiguration(c, err);
}

bool ConfigurationManager::setConfiguration(const Configuration& c, String& errorMessage)
{	
	if ((int)c.sessionStoreIntervalSecs.getValue() < 5)
	{
		errorMessage = "Session store interval is too low. Minimal interval is 5 seconds";
		return false;
	}

	const String wlanPass = c.WLANPassword.getValue().toString();
	if (wlanPass.removeCharacters(" \n\r").length() != wlanPass.length())
	{
		errorMessage = "WLAN password must not contain whitespace characters";
		return false;
	}

	if (wlanPass.length() < 8)
	{
		errorMessage = "WLAN password is too short. Minimal length is 8 characters";
		return false;
	}

	const String adminPass = c.adminPassword.getValue().toString();
	if (adminPass.removeCharacters(" \n\r").length() != adminPass.length())
	{
		errorMessage = "Administrator password must not contain whitespace characters";
		return false;
	}

	if (adminPass.length() < 4)
	{
		errorMessage = "Administrator password is too short. Minimal length is 4 characters";
		return false;
	}

	ScopedPointer<Configuration> oldConfig = config.release();
	config = new Configuration(c);

	listeners.call(&Listener::configurationChanged, *oldConfig, *config);
	return true;
}

bool ConfigurationManager::saveConfig() const
{
	return saveConfigToFile(Main::getInstanceCast()->getApplicationConfigFile());
}

bool ConfigurationManager::saveConfigToFile(const File& storeFile) const
{
	const ScopedPointer<XmlElement> stateXml = getState().createXml();
	if (stateXml == nullptr)
	{
		jassertfalse;
		return false;
	}

	if (!storeFile.hasWriteAccess())
	{
		jassertfalse;
		return false;
	}

	return stateXml->writeToFile(storeFile, "", "UTF-8", 10);
}

bool ConfigurationManager::loadConfig()
{
	const File configFile = Main::getInstanceCast()->getApplicationConfigFile();
	if (!configFile.existsAsFile())
		return false;

	loadConfigFromFile(configFile);
	return true;
}

void ConfigurationManager::loadConfigFromFile(const File& file)
{
	const ScopedPointer<XmlElement> stateXml = XmlDocument::parse(file);
	setState(ValueTree::fromXml(*stateXml));
}

ValueTree ConfigurationManager::getState() const
{
	return config->getState();
}

void ConfigurationManager::setState(const ValueTree& state)
{
	Configuration c = getDefaultConfiguration();
	c.setState(state);
	setConfiguration(c);
}
