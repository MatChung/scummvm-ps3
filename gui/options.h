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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/gui/options.h $
 * $Id: options.h 54090 2010-11-05 10:53:37Z thebluegr $
 */

#ifndef OPTIONS_DIALOG_H
#define OPTIONS_DIALOG_H

#include "gui/dialog.h"
#include "gui/TabWidget.h"
#include "common/str.h"
#include "sound/musicplugin.h"

#ifdef SMALL_SCREEN_DEVICE
#include "gui/KeysDialog.h"
#endif

namespace GUI {

class BrowserDialog;
class CheckboxWidget;
class PopUpWidget;
class SliderWidget;
class StaticTextWidget;
class ListWidget;

class OptionsDialog : public Dialog {
public:
	OptionsDialog(const Common::String &domain, int x, int y, int w, int h);
	OptionsDialog(const Common::String &domain, const Common::String &name);

	void init();

	void open();
	void close();
	void handleCommand(CommandSender *sender, uint32 cmd, uint32 data);
	const Common::String& getDomain() const { return _domain; }

	virtual void reflowLayout();

protected:
	/** Config domain this dialog is used to edit. */
	Common::String _domain;

	ButtonWidget *_soundFontButton;
	StaticTextWidget *_soundFont;
	ButtonWidget *_soundFontClearButton;

	void addGraphicControls(GuiObject *boss, const Common::String &prefix);
	void addAudioControls(GuiObject *boss, const Common::String &prefix);
	void addMIDIControls(GuiObject *boss, const Common::String &prefix);
	void addMT32Controls(GuiObject *boss, const Common::String &prefix);
	void addVolumeControls(GuiObject *boss, const Common::String &prefix);
	// The default value is the launcher's non-scaled talkspeed value. When SCUMM uses the widget,
	// it uses its own scale
	void addSubtitleControls(GuiObject *boss, const Common::String &prefix, int maxSliderVal = 255);

	void setGraphicSettingsState(bool enabled);
	void setAudioSettingsState(bool enabled);
	void setMIDISettingsState(bool enabled);
	void setMT32SettingsState(bool enabled);
	void setVolumeSettingsState(bool enabled);
	void setSubtitleSettingsState(bool enabled);

	bool loadMusicDeviceSetting(PopUpWidget *popup, Common::String setting, MusicType preferredType = MT_AUTO);
	void saveMusicDeviceSetting(PopUpWidget *popup, Common::String setting);

	TabWidget *_tabWidget;
	int _graphicsTabId;

private:
	//
	// Graphics controls
	//
	bool _enableGraphicSettings;
	StaticTextWidget *_gfxPopUpDesc;
	PopUpWidget *_gfxPopUp;
	CheckboxWidget *_fullscreenCheckbox;
	CheckboxWidget *_aspectCheckbox;
	CheckboxWidget *_disableDitheringCheckbox;
	StaticTextWidget *_renderModePopUpDesc;
	PopUpWidget *_renderModePopUp;

	//
	// Audio controls
	//
	bool _enableAudioSettings;
	StaticTextWidget *_midiPopUpDesc;
	PopUpWidget *_midiPopUp;
	StaticTextWidget *_oplPopUpDesc;
	PopUpWidget *_oplPopUp;
	StaticTextWidget *_outputRatePopUpDesc;
	PopUpWidget *_outputRatePopUp;

	StaticTextWidget *_mt32DevicePopUpDesc;
	PopUpWidget *_mt32DevicePopUp;
	StaticTextWidget *_gmDevicePopUpDesc;
	PopUpWidget *_gmDevicePopUp;



	//
	// MIDI controls
	//
	bool _enableMIDISettings;
	CheckboxWidget *_multiMidiCheckbox;
	StaticTextWidget *_midiGainDesc;
	SliderWidget *_midiGainSlider;
	StaticTextWidget *_midiGainLabel;

	//
	// MT-32 controls
	//
	bool _enableMT32Settings;
	CheckboxWidget *_mt32Checkbox;
	CheckboxWidget *_enableGSCheckbox;

	//
	// Subtitle controls
	//
	int getSubtitleMode(bool subtitles, bool speech_mute);
	bool _enableSubtitleSettings;
	StaticTextWidget *_subToggleDesc;
	RadiobuttonGroup *_subToggleGroup;
	RadiobuttonWidget *_subToggleSubOnly;
	RadiobuttonWidget *_subToggleSpeechOnly;
	RadiobuttonWidget *_subToggleSubBoth;
	static const char *_subModeDesc[];
	static const char *_lowresSubModeDesc[];
	StaticTextWidget *_subSpeedDesc;
	SliderWidget *_subSpeedSlider;
	StaticTextWidget *_subSpeedLabel;

	//
	// Volume controls
	//
	bool _enableVolumeSettings;

	StaticTextWidget *_musicVolumeDesc;
	SliderWidget *_musicVolumeSlider;
	StaticTextWidget *_musicVolumeLabel;

	StaticTextWidget *_sfxVolumeDesc;
	SliderWidget *_sfxVolumeSlider;
	StaticTextWidget *_sfxVolumeLabel;

	StaticTextWidget *_speechVolumeDesc;
	SliderWidget *_speechVolumeSlider;
	StaticTextWidget *_speechVolumeLabel;

	CheckboxWidget *_muteCheckbox;

protected:
	//
	// Game GUI options
	//
	uint32 _guioptions;
	Common::String _guioptionsString;
};


class GlobalOptionsDialog : public OptionsDialog {
public:
	GlobalOptionsDialog();
	~GlobalOptionsDialog();

	void open();
	void close();
	void handleCommand(CommandSender *sender, uint32 cmd, uint32 data);

protected:
#ifdef SMALL_SCREEN_DEVICE
	KeysDialog *_keysDialog;
#endif
	StaticTextWidget *_savePath;
	StaticTextWidget *_themePath;
	StaticTextWidget *_extraPath;
#ifdef DYNAMIC_MODULES
	StaticTextWidget *_pluginsPath;
#endif

	//
	// Misc controls
	//
	StaticTextWidget *_curTheme;
	StaticTextWidget *_rendererPopUpDesc;
	PopUpWidget *_rendererPopUp;
	StaticTextWidget *_autosavePeriodPopUpDesc;
	PopUpWidget *_autosavePeriodPopUp;
	StaticTextWidget *_guiLanguagePopUpDesc;
	PopUpWidget *_guiLanguagePopUp;
};

} // End of namespace GUI

#endif
