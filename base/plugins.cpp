/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/base/plugins.cpp $
 * $Id: plugins.cpp 54097 2010-11-05 13:24:57Z bluddy $
 *
 */

#include "base/plugins.h"

#include "common/func.h"
#include "common/debug.h"

#ifdef DYNAMIC_MODULES
#include "common/config-manager.h"
#include "common/fs.h"
#endif

// Plugin versioning

int pluginTypeVersions[PLUGIN_TYPE_MAX] = {
	PLUGIN_TYPE_ENGINE_VERSION,
	PLUGIN_TYPE_MUSIC_VERSION,
};


// Abstract plugins

PluginType Plugin::getType() const {
	return _type;
}

const char *Plugin::getName() const {
	return _pluginObject->getName();
}

class StaticPlugin : public Plugin {
public:
	StaticPlugin(PluginObject *pluginobject, PluginType type) {
		assert(pluginobject);
		assert(type < PLUGIN_TYPE_MAX);
		_pluginObject = pluginobject;
		_type = type;
	}

	~StaticPlugin() {
		delete _pluginObject;
	}

	virtual bool loadPlugin()		{ return true; }
	virtual void unloadPlugin()		{}
};

class StaticPluginProvider : public PluginProvider {
public:
	StaticPluginProvider() {
	}

	~StaticPluginProvider() {
	}

	virtual PluginList getPlugins() {
		PluginList pl;

		#define LINK_PLUGIN(ID) \
			extern PluginType g_##ID##_type; \
			extern PluginObject *g_##ID##_getObject(); \
			pl.push_back(new StaticPlugin(g_##ID##_getObject(), g_##ID##_type));

		// "Loader" for the static plugins.
		// Iterate over all registered (static) plugins and load them.

		// Engine plugins
		#if PLUGIN_ENABLED_STATIC(SCUMM)
		LINK_PLUGIN(SCUMM)
		#endif
		#if PLUGIN_ENABLED_STATIC(AGI)
		LINK_PLUGIN(AGI)
		#endif
		#if PLUGIN_ENABLED_STATIC(AGOS)
		LINK_PLUGIN(AGOS)
		#endif
		#if PLUGIN_ENABLED_STATIC(CINE)
		LINK_PLUGIN(CINE)
		#endif
		#if PLUGIN_ENABLED_STATIC(CRUISE)
		LINK_PLUGIN(CRUISE)
		#endif
		#if PLUGIN_ENABLED_STATIC(DRACI)
		LINK_PLUGIN(DRACI)
		#endif
		#if PLUGIN_ENABLED_STATIC(DRASCULA)
		LINK_PLUGIN(DRASCULA)
		#endif
		#if PLUGIN_ENABLED_STATIC(GOB)
		LINK_PLUGIN(GOB)
		#endif
		#if PLUGIN_ENABLED_STATIC(GROOVIE)
		LINK_PLUGIN(GROOVIE)
		#endif
		#if PLUGIN_ENABLED_STATIC(HUGO)
		LINK_PLUGIN(HUGO)
		#endif
		#if PLUGIN_ENABLED_STATIC(KYRA)
		LINK_PLUGIN(KYRA)
		#endif
		#if PLUGIN_ENABLED_STATIC(LASTEXPRESS)
		LINK_PLUGIN(LASTEXPRESS)
		#endif
		#if PLUGIN_ENABLED_STATIC(LURE)
		LINK_PLUGIN(LURE)
		#endif
		#if PLUGIN_ENABLED_STATIC(M4)
		LINK_PLUGIN(M4)
		#endif
		#if PLUGIN_ENABLED_STATIC(MADE)
		LINK_PLUGIN(MADE)
		#endif
		#if PLUGIN_ENABLED_STATIC(MOHAWK)
		LINK_PLUGIN(MOHAWK)
		#endif
		#if PLUGIN_ENABLED_STATIC(PARALLACTION)
		LINK_PLUGIN(PARALLACTION)
		#endif
		#if PLUGIN_ENABLED_STATIC(QUEEN)
		LINK_PLUGIN(QUEEN)
		#endif
		#if PLUGIN_ENABLED_STATIC(SAGA)
		LINK_PLUGIN(SAGA)
		#endif
		#if PLUGIN_ENABLED_STATIC(SCI)
		LINK_PLUGIN(SCI)
		#endif
		#if PLUGIN_ENABLED_STATIC(SKY)
		LINK_PLUGIN(SKY)
		#endif
		#if PLUGIN_ENABLED_STATIC(SWORD1)
		LINK_PLUGIN(SWORD1)
		#endif
		#if PLUGIN_ENABLED_STATIC(SWORD2)
		LINK_PLUGIN(SWORD2)
		#endif
		#if PLUGIN_ENABLED_STATIC(SWORD25)
		LINK_PLUGIN(SWORD25)
		#endif
		#if PLUGIN_ENABLED_STATIC(TEENAGENT)
		LINK_PLUGIN(TEENAGENT)
		#endif
		#if PLUGIN_ENABLED_STATIC(TESTBED)
		LINK_PLUGIN(TESTBED)
		#endif
		#if PLUGIN_ENABLED_STATIC(TINSEL)
		LINK_PLUGIN(TINSEL)
		#endif
		#if PLUGIN_ENABLED_STATIC(TOON)
		LINK_PLUGIN(TOON)
		#endif
		#if PLUGIN_ENABLED_STATIC(TOUCHE)
		LINK_PLUGIN(TOUCHE)
		#endif
		#if PLUGIN_ENABLED_STATIC(TUCKER)
		LINK_PLUGIN(TUCKER)
		#endif

		// Music plugins
		// TODO: Use defines to disable or enable each MIDI driver as a
		// static/dynamic plugin, like it's done for the engines
		LINK_PLUGIN(AUTO)
		LINK_PLUGIN(NULL)
		#if defined(WIN32) && !defined(_WIN32_WCE) && !defined(__SYMBIAN32__)
		LINK_PLUGIN(WINDOWS)
		#endif
		#if defined(USE_ALSA)
		LINK_PLUGIN(ALSA)
		#endif
		#if defined(USE_SEQ_MIDI)
		LINK_PLUGIN(SEQ)
		#endif
		#if defined(__MINT__)
		LINK_PLUGIN(STMIDI)
		#endif
		#if defined(IRIX)
		LINK_PLUGIN(DMEDIA)
		#endif
		#if defined(__amigaos4__)
		LINK_PLUGIN(CAMD)
		#endif
		#if defined(MACOSX)
		LINK_PLUGIN(COREAUDIO)
		LINK_PLUGIN(COREMIDI)
		#endif
		#ifdef USE_FLUIDSYNTH
		LINK_PLUGIN(FLUIDSYNTH)
		#endif
		#ifdef USE_MT32EMU
		LINK_PLUGIN(MT32)
		#endif
		LINK_PLUGIN(ADLIB)
		LINK_PLUGIN(PCSPK)
		LINK_PLUGIN(PCJR)
		LINK_PLUGIN(CMS)
		#ifndef DISABLE_SID
		LINK_PLUGIN(C64)
		#endif
		LINK_PLUGIN(AMIGA)
		LINK_PLUGIN(APPLEIIGS)
		LINK_PLUGIN(TOWNS)
		#if defined(USE_TIMIDITY)
		LINK_PLUGIN(TIMIDITY)
		#endif

		return pl;
	}
};

#ifdef DYNAMIC_MODULES

PluginList FilePluginProvider::getPlugins() {
	PluginList pl;

	// Prepare the list of directories to search
	Common::FSList pluginDirs;

	// Add the default directories
	pluginDirs.push_back(Common::FSNode("."));
	pluginDirs.push_back(Common::FSNode("plugins"));

	// Add the provider's custom directories
	addCustomDirectories(pluginDirs);

	// Add the user specified directory
	Common::String pluginsPath(ConfMan.get("pluginspath"));
	if (!pluginsPath.empty())
		pluginDirs.push_back(Common::FSNode(pluginsPath));

	Common::FSList::const_iterator dir;
	for (dir = pluginDirs.begin(); dir != pluginDirs.end(); ++dir) {
		// Load all plugins.
		// Scan for all plugins in this directory
		Common::FSList files;
		if (!dir->getChildren(files, Common::FSNode::kListFilesOnly)) {
			debug(1, "Couldn't open plugin directory '%s'", dir->getPath().c_str());
			continue;
		} else {
			debug(1, "Reading plugins from plugin directory '%s'", dir->getPath().c_str());
		}

		for (Common::FSList::const_iterator i = files.begin(); i != files.end(); ++i) {
			if (isPluginFilename(*i)) {
				pl.push_back(createPlugin(*i));
			}
		}
	}

	return pl;
}

bool FilePluginProvider::isPluginFilename(const Common::FSNode &node) const {
	Common::String filename = node.getName();

#ifdef PLUGIN_PREFIX
	// Check the plugin prefix
	if (!filename.hasPrefix(PLUGIN_PREFIX))
		return false;
#endif

#ifdef PLUGIN_SUFFIX
	// Check the plugin suffix
	if (!filename.hasSuffix(PLUGIN_SUFFIX))
		return false;
#endif

	return true;
}

void FilePluginProvider::addCustomDirectories(Common::FSList &dirs) const {
#ifdef PLUGIN_DIRECTORY
	dirs.push_back(Common::FSNode(PLUGIN_DIRECTORY));
#endif
}

#endif // DYNAMIC_MODULES

#pragma mark -

DECLARE_SINGLETON(PluginManager)

PluginManager::PluginManager() {
	// Always add the static plugin provider.
	addPluginProvider(new StaticPluginProvider());
}

PluginManager::~PluginManager() {
	// Explicitly unload all loaded plugins
	unloadPlugins();

	// Delete the plugin providers
	for (ProviderList::iterator pp = _providers.begin();
	                            pp != _providers.end();
	                            ++pp) {
		delete *pp;
	}
}

void PluginManager::addPluginProvider(PluginProvider *pp) {
	_providers.push_back(pp);
}

//
// This should only be run once
void PluginManager::loadNonEnginePluginsAndEnumerate() {
	unloadPlugins();
	_allEnginePlugins.clear();
	
	// We need to resize our pluginsInMem list to prevent fragmentation
	// Otherwise, as soon as we add our 1 engine plugin (which is all we'll have in memory at a time)
	// We'll get an allocation in memory that will never go away
	_pluginsInMem[PLUGIN_TYPE_ENGINE].resize(2);	// more than we need

	for (ProviderList::iterator pp = _providers.begin();
	                            pp != _providers.end();
	                            ++pp) {
		PluginList pl((*pp)->getPlugins());
		for (PluginList::iterator p = pl.begin(); p != pl.end(); ++p) {
			// To find out which are engine plugins, we have to load them. This is inefficient
			// Hopefully another way can be found (e.g. if the music plugins are all static, 
			// we can use only the static provider
			if ((*p)->loadPlugin()) {
				if ((*p)->getType() == PLUGIN_TYPE_ENGINE) {
					(*p)->unloadPlugin();				// to prevent fragmentation
					_allEnginePlugins.push_back(*p);
				} else {	// add non-engine plugins to the 'in-memory' list
							// these won't ever get unloaded (in this implementation)
					addToPluginsInMemList(*p);			
				}
			}	
 		}
 	}
}

void PluginManager::loadFirstPlugin() { 
	unloadPluginsExcept(PLUGIN_TYPE_ENGINE, NULL, false);

	// let's try to find one we can load
	for (_currentPlugin = _allEnginePlugins.begin(); _currentPlugin != _allEnginePlugins.end(); ++_currentPlugin) {
		if ((*_currentPlugin)->loadPlugin()) {
			addToPluginsInMemList(*_currentPlugin);
			break;
		}
	}
}

bool PluginManager::loadNextPlugin() {
	unloadPluginsExcept(PLUGIN_TYPE_ENGINE, NULL, false);

	for (++_currentPlugin; _currentPlugin != _allEnginePlugins.end(); ++_currentPlugin) {
		if ((*_currentPlugin)->loadPlugin()) {
			addToPluginsInMemList(*_currentPlugin);
			return true;
		}
	}
	return false;	// no more in list
}

void PluginManager::loadPlugins() {
	for (ProviderList::iterator pp = _providers.begin();
	                            pp != _providers.end();
	                            ++pp) {
		PluginList pl((*pp)->getPlugins());
		Common::for_each(pl.begin(), pl.end(), Common::bind1st(Common::mem_fun(&PluginManager::tryLoadPlugin), this));
	}
}

void PluginManager::unloadPlugins() {
	for (int i = 0; i < PLUGIN_TYPE_MAX; i++)
		unloadPluginsExcept((PluginType)i, NULL);
}

void PluginManager::unloadPluginsExcept(PluginType type, const Plugin *plugin, bool deletePlugin /*=true*/) {
	Plugin *found = NULL;
	for (PluginList::iterator p = _pluginsInMem[type].begin(); p != _pluginsInMem[type].end(); ++p) {
		if (*p == plugin) {
			found = *p;
		} else {
			(*p)->unloadPlugin();
			if (deletePlugin)
				delete *p;
		}
	}
	_pluginsInMem[type].clear();
	if (found != NULL) {
		_pluginsInMem[type].push_back(found);
	}
}

bool PluginManager::tryLoadPlugin(Plugin *plugin) {
	assert(plugin);
	// Try to load the plugin
	if (plugin->loadPlugin()) {
		addToPluginsInMemList(plugin);
		return true;
	} else {
		// Failed to load the plugin
		delete plugin;
		return false;
	}
}

void PluginManager::addToPluginsInMemList(Plugin *plugin) {
	bool found = false;
	// The plugin is valid, see if it provides the same module as an
	// already loaded one and should replace it.
		
	PluginList::iterator pl = _pluginsInMem[plugin->getType()].begin();
	while (!found && pl != _pluginsInMem[plugin->getType()].end()) {
		if (!strcmp(plugin->getName(), (*pl)->getName())) {
			// Found a duplicated module. Replace the old one.
			found = true;
			delete *pl;
			*pl = plugin;
			debug(1, "Replaced the duplicated plugin: '%s'", plugin->getName());
		}
		pl++;
	}

	if (!found) {
		// If it provides a new module, just add it to the list of known plugins in memory.
		_pluginsInMem[plugin->getType()].push_back(plugin);
	}
}

// Engine plugins

#include "engines/metaengine.h"

DECLARE_SINGLETON(EngineManager)

GameDescriptor EngineManager::findGameOnePluginAtATime(const Common::String &gameName, const EnginePlugin **plugin) const {
	GameDescriptor result;
	PluginManager::instance().loadFirstPlugin();
	do {
		result = findGame(gameName, plugin); 
		if (!result.gameid().empty()) {
			break;
		}
	} while (PluginManager::instance().loadNextPlugin());
	return result;
}

GameDescriptor EngineManager::findGame(const Common::String &gameName, const EnginePlugin **plugin) const {
	// Find the GameDescriptor for this target
	const EnginePlugin::List &plugins = getPlugins();
	GameDescriptor result;

	if (plugin)
		*plugin = 0;

	EnginePlugin::List::const_iterator iter;
	
	for (iter = plugins.begin(); iter != plugins.end(); ++iter) {
		result = (**iter)->findGame(gameName.c_str());
		if (!result.gameid().empty()) {
			if (plugin)
				*plugin = *iter;
			return result;
		}
	}
	return result;
}

GameList EngineManager::detectGames(const Common::FSList &fslist) const {
	GameList candidates;
	EnginePlugin::List plugins;
	EnginePlugin::List::const_iterator iter;
#if defined(ONE_PLUGIN_AT_A_TIME) && defined(DYNAMIC_MODULES)
	PluginManager::instance().loadFirstPlugin();
	do {
#endif
		plugins = getPlugins();
		// Iterate over all known games and for each check if it might be
		// the game in the presented directory.
		for (iter = plugins.begin(); iter != plugins.end(); ++iter) {
			candidates.push_back((**iter)->detectGames(fslist));
		}
#if defined(ONE_PLUGIN_AT_A_TIME) && defined(DYNAMIC_MODULES)
	} while (PluginManager::instance().loadNextPlugin());
#endif
	return candidates;
}

const EnginePlugin::List &EngineManager::getPlugins() const {
	return (const EnginePlugin::List &)PluginManager::instance().getPlugins(PLUGIN_TYPE_ENGINE);
}


// Music plugins

#include "sound/musicplugin.h"

DECLARE_SINGLETON(MusicManager)

const MusicPlugin::List &MusicManager::getPlugins() const {
	return (const MusicPlugin::List &)PluginManager::instance().getPlugins(PLUGIN_TYPE_MUSIC);
}
