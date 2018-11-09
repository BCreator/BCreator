// ******************************************************************************
// Filename:    VoxSettings.cpp
// Project:     Vox
// Author:      Steven Ball
//
// Revision History:
//   Initial Revision - 15/11/15
//
// Copyright (c) 2005-2015, Steven Ball
// ******************************************************************************

#include "VoxSettings.h"

#include <iostream>
#include "ini/INIReader.h"

#include <fstream>
#include <ostream>
#include <iostream>
#include <string>
using namespace std;


VoxSettings::VoxSettings()
{
}

VoxSettings::~VoxSettings()
{
}

// Load settings
void VoxSettings::LoadSettings()
{
	string settingsIniFile = "media/config/settings.ini";
	INIReader reader(settingsIniFile);

	if (reader.ParseError() < 0)
	{
		cout << "Can't load '" << settingsIniFile << "'\n";
		return;
	}

	// Graphics
	m_windowWidth = reader.GetInteger("Graphics", "WindowWidth", 800);
	m_windowHeight = reader.GetInteger("Graphics", "WindowHeight", 800);
	m_vsync = reader.GetBoolean("Graphics", "VSync", false);
	m_fullscreen = reader.GetBoolean("Graphics", "FullScreen", false);
	m_deferredRendering = reader.GetBoolean("Graphics", "DeferredRendering", false);
	m_blur = reader.GetBoolean("Graphics", "Blur", false);
	m_ssao = reader.GetBoolean("Graphics", "SSAO", false);
	m_dynamicLighting = reader.GetBoolean("Graphics", "DynamicLighting", false);
	m_msaa = reader.GetBoolean("Graphics", "MSAA", false);
	m_instancedParticles = reader.GetBoolean("Graphics", "InstancedParticles", false);
	m_faceMerging = reader.GetBoolean("Graphics", "FaceMerging", false);

	// Landscape generation
	m_landscapeOctaves = (float)reader.GetReal("Landscape", "LandscapeOctaves", 4.0f);
	m_landscapePersistence = (float)reader.GetReal("Landscape", "LandscapePersistance", 0.3f);
	m_landscapeScale = (float)reader.GetReal("Landscape", "LandscapeScale", 0.01f);
	m_mountainOctaves = (float)reader.GetReal("Landscape", "MountainOctaves", 4.0f);
	m_mountainPersistence = (float)reader.GetReal("Landscape", "MountainPersistance", 0.3f);
	m_mountainScale = (float)reader.GetReal("Landscape", "MountainScale", 0.0075f);
	m_mountainMultiplier = (float)reader.GetReal("Landscape", "MountainMultiplier", 3.0f);

	// Debug
	m_loaderRadius = (float)reader.GetReal("Debug", "LoaderRadius", 64.0f);
	m_debugRendering = reader.GetBoolean("Debug", "DebugRendering", false);
	m_stepUpdating = reader.GetBoolean("Debug", "StepUpdatng", false);
	m_wireframeRendering = reader.GetBoolean("Debug", "WireframeRendering", false);
	m_showDebugGUI = reader.GetBoolean("Debug", "ShowDebugGUI", true);
	m_gameMode = reader.Get("Debug", "GameMode", "Debug");
	m_version = reader.Get("Debug", "Version", "1.0");
}

// Save settings
void VoxSettings::SaveSettings()
{
}

// Load options
void VoxSettings::LoadOptions()
{
	string optionsIniFile = "media/config/options.ini";
	INIReader reader(optionsIniFile);

	if (reader.ParseError() < 0)
	{
		cout << "Can't load '" << optionsIniFile << "'\n";
		return;
	}

	// Gameplay 
	m_invertedMouse = reader.GetBoolean("Gameplay", "InvertedMouse", false);
	m_mouseSensitivity = (float)reader.GetReal("Gameplay", "MouseSensitivity", 50.0f);
	m_gamepadSensitivity = (float)reader.GetReal("Gameplay", "GamepadSensitivity", 50.0f);

	// Graphics
	m_shadows = reader.GetBoolean("Graphics", "Shadows", false);
	m_fogRendering = reader.GetBoolean("Graphics", "FogRendering", false);
	m_waterRendering = reader.GetBoolean("Graphics", "WaterRendering", false);

	// Sound
	m_audio = reader.GetBoolean("Sound", "AudioEnabled", true);
	m_audioVolume = (float)reader.GetReal("Sound", "AudioVolume", 0.5f);
	m_music = reader.GetBoolean("Sound", "MusicEnabled", true);
	m_musicVolume = (float)reader.GetReal("Sound", "MusicVolume", 0.5f);

	// Interface
	m_renderGUI = reader.GetBoolean("Interface", "RenderGUI", true);
	m_renderCrosshair = reader.GetBoolean("Interface", "RenderCrosshair", true);
	m_HUDAnimations = reader.GetBoolean("Interface", "HUDAnimationd", true);
	m_customCursors = reader.GetBoolean("Interface", "CustomCursors", true);
	m_equipHelper = reader.GetBoolean("Interface", "EquipHelper", true);
	m_tooltipDelay = reader.GetBoolean("Interface", "TooltipDelay", true);
	m_confirmItemDelete = reader.GetBoolean("Interface", "ConfirmItemDelete", true);

	// Controls
}

// Save options
void VoxSettings::SaveOptions()
{
	ofstream file;

	// Open the file
	string optionsIniFile = "media/config/options.ini";
	file.open(optionsIniFile.c_str(), ios::out);

	file << "[Gameplay]\n";
	file << "InvertedMouse=" << (m_invertedMouse ? "True" : "False") << "\n";	
	file << "MouseSensitivity=" << m_mouseSensitivity << "\n";
	file << "GamepadSensitivity=" << m_gamepadSensitivity << "\n";
	file << "\n";

	file << "[Graphics]\n";
	file << "Shadows=" << (m_shadows ? "True" : "False") << "\n";
	file << "FogRendering=" << (m_fogRendering ? "True" : "False") << "\n";
	file << "WaterRendering=" << (m_waterRendering ? "True" : "False") << "\n";
	file << "\n";

	file << "[Sound]\n";
	file << "AudioEnabled=" << (m_audio ? "True" : "False") << "\n";
	file << "AudioVolume=" << m_audioVolume << "\n";
	file << "MusicEnabled=" << (m_music ? "True" : "False") << "\n";
	file << "MusicVolume=" << m_musicVolume << "\n";
	file << "\n";

	file << "[Interface]\n";
	file << "RenderGUI=" << (m_renderGUI ? "True" : "False") << "\n";
	file << "RenderCrosshair=" << (m_renderCrosshair ? "True" : "False") << "\n";
	file << "HUDAnimationd=" << (m_HUDAnimations ? "True" : "False") << "\n";
	file << "CustomCursors=" << (m_customCursors ? "True" : "False") << "\n";
	file << "EquipHelper=" << (m_equipHelper ? "True" : "False") << "\n";
	file << "TooltipDelay=" << (m_tooltipDelay ? "True" : "False") << "\n";
	file << "ConfirmItemDelete=" << (m_confirmItemDelete ? "True" : "False") << "\n";
	file << "\n";

	file << "[Controls]\n";
	file << "\n";
}