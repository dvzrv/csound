/*
    CsoundPerformanceSettings.cpp:
    Copyright (C) 2006 Istvan Varga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include "CsoundGUI.hpp"

CsoundPerformanceSettings::CsoundPerformanceSettings()
{
    orcName = "";
    scoName = "";
    soundFileType = "wav";
    soundSampleFormat = "short";
    enablePeakChunks = true;
    displayMode = 1;
    deferGEN1 = false;
    bufFrames_SW = 1024;
    nBuffers = 4;
    midiInFileName = "";
    midiOutFileName = "";
    midiInDevName = "";
    midiOutDevName = "";
    terminateOnMidi = false;
    heartBeatMode = 0;
    rewriteHeader = false;
    inputFileName = "";
    outputFileName = "dac";
    enableSoundOutput = true;
    beatModeTempo = -1.0;
    iTimeOnly = false;
    sampleRateOverride = -1.0;
    controlRateOverride = -1.0;
    lineInput = "";
    messageLevel = 231;
    enableExpressionOpt = false;
    sadirPath = "";
    ssdirPath = "";
    sfdirPath = "";
    incdirPath = "";
    for (int i = 0; i < 10; i++)
      strsets[i] = "";
    verbose = false;
    enableDither = false;
    pluginLibs = "";
    sndidArtist = "";
    sndidComment = "";
    sndidCopyright = "";
    sndidDate = "";
    sndidSoftware = "";
    sndidTitle = "";
    ignoreCSDOptions = false;
    jackClientName = "";
    jackInPortName = "";
    jackOutPortName = "";
    maxStrLen = 255;
    midiFileMuteTracks = "";
    rawControllerMode = false;
    rtAudioModule = "";
    rtMidiModule = "";
    scoreOffsetSeconds = 0.0;
    useThreads = true;
}

CsoundPerformanceSettings::~CsoundPerformanceSettings()
{
}

static const char *fileTypeList[] = {
    "wav", "aiff", "au", "raw", "ircam", "w64", "wavex", "sd2", "flac",
    (char*) 0
};

static const char *sampleFormatList[] = {
    "alaw", "ulaw", "schar", "uchar", "float", "short", "long", "24bit",
    (char*) 0
};

int CsoundPerformanceSettings::fileTypeToIndex(const char *fileType)
{
    if (fileType == (char*) 0 || fileType[0] == (char) 0)
      return -1;
    for (int i = 0; fileTypeList[i] != (char*) 0; i++) {
      if (std::strcmp(fileTypeList[i], fileType) == 0)
        return i;
    }
    return -1;
}

const char *CsoundPerformanceSettings::indexToFileType(int fileType)
{
    if (fileType < 0 ||
        fileType >= (int) (sizeof(fileTypeList) / sizeof(char*)))
      return (char*) 0;
    return fileTypeList[fileType];
}

int CsoundPerformanceSettings::sampleFormatToIndex(const char *sampleFormat)
{
    if (sampleFormat == (char*) 0 || sampleFormat[0] == (char) 0)
      return -1;
    for (int i = 0; sampleFormatList[i] != (char*) 0; i++) {
      if (std::strcmp(sampleFormatList[i], sampleFormat) == 0)
        return i;
    }
    return -1;
}

const char *CsoundPerformanceSettings::indexToSampleFormat(int sampleFormat)
{
    if (sampleFormat < 0 ||
        sampleFormat >= (int) (sizeof(sampleFormatList) / sizeof(char*)))
      return (char*) 0;
    return sampleFormatList[sampleFormat];
}

static bool cmdLine_addStringOpt(std::vector<std::string>& cmdLine,
                                 const char *optName, std::string& value)
{
    std::string arg;
    int         i, pos0, pos1;

    for (pos0 = 0; pos0 < (int) value.size(); pos0++) {
      char  c = value[pos0];
      if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
        break;
    }
    for (pos1 = (int) value.size() - 1; pos1 >= 0; pos1--) {
      char  c = value[pos1];
      if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
        break;
    }
    if (pos0 > pos1)
      return false;
    arg = optName;
    for (i = pos0; i <= pos1; i++)
      arg += value[i];
    cmdLine.push_back(arg);

    return true;
}

static void cmdLine_addIntegerOpt(std::vector<std::string>& cmdLine,
                                  const char *optName, int value)
{
    char    buf[64];

    std::sprintf(&(buf[0]), "%s%d", optName, value);
    cmdLine.push_back(&(buf[0]));
}

static void cmdLine_addDoubleOpt(std::vector<std::string>& cmdLine,
                                 const char *optName, double value)
{
    char    buf[64];

    if (!(value > -10000000.0 && value < 10000000.0))
      return;
    std::sprintf(&(buf[0]), "%s%g", optName, value);
    cmdLine.push_back(&(buf[0]));
}

void CsoundPerformanceSettings::buildCommandLine(std::vector<std::string>&
                                                     cmdLine,
                                                 bool forceSettings)
{
    cmdLine.clear();
    cmdLine.push_back("csound");
    if (!CsoundGUIMain::isEmptyString(soundFileType) &&
        (soundFileType != "wav" || forceSettings)) {
      std::string arg;
      arg = "--format=";
      arg += soundFileType;
      if (!CsoundGUIMain::isEmptyString(soundSampleFormat) &&
          (soundSampleFormat != "short" || forceSettings)) {
        arg += ':';
        arg += soundSampleFormat;
      }
      cmdLine.push_back(arg);
    }
    else if (!CsoundGUIMain::isEmptyString(soundSampleFormat) &&
             (soundSampleFormat != "short" || forceSettings)) {
      cmdLine_addStringOpt(cmdLine, "--format=", soundSampleFormat);
    }
    if (!enablePeakChunks) {
      cmdLine.push_back("-K");
    }
    if (displayMode != 1 || forceSettings) {
      switch (displayMode) {    // 0: none, 1: full, 2: ASCII, 3: PS
      case 0:
        cmdLine.push_back("-d");
        break;
      case 1:
        cmdLine.push_back("--displays");
        break;
      case 2:
        if (forceSettings)
          cmdLine.push_back("--displays");
        cmdLine.push_back("-g");
        break;
      case 3:
        if (forceSettings)
          cmdLine.push_back("--displays");
        cmdLine.push_back("-G");
        break;
      }
    }
    if (deferGEN1)
      cmdLine.push_back("-D");
    if (bufFrames_SW >= 16) {
      cmdLine_addIntegerOpt(cmdLine, "-b", bufFrames_SW);
      cmdLine_addIntegerOpt(cmdLine, "-B", bufFrames_SW * nBuffers);
    }
    cmdLine_addStringOpt(cmdLine, "-F", midiInFileName);
    cmdLine_addStringOpt(cmdLine, "--midioutfile=", midiOutFileName);
    cmdLine_addStringOpt(cmdLine, "-M", midiInDevName);
    cmdLine_addStringOpt(cmdLine, "-Q", midiOutDevName);
    if (terminateOnMidi)
      cmdLine.push_back("-T");
    if (heartBeatMode != 0 || forceSettings)
      cmdLine_addIntegerOpt(cmdLine, "-H", heartBeatMode);
    if (rewriteHeader)
      cmdLine.push_back("-R");
    cmdLine_addStringOpt(cmdLine, "-i", inputFileName);
    if (!CsoundGUIMain::isEmptyString(outputFileName) && enableSoundOutput)
      cmdLine_addStringOpt(cmdLine, "-o", outputFileName);
    else if (forceSettings)
      cmdLine.push_back("-n");
    if (beatModeTempo > 0.0)
      cmdLine_addDoubleOpt(cmdLine, "-t", beatModeTempo);
    if (iTimeOnly)
      cmdLine.push_back("-I");
    if (sampleRateOverride > 0.0 && controlRateOverride > 0.0) {
      cmdLine_addDoubleOpt(cmdLine, "-r", sampleRateOverride);
      cmdLine_addDoubleOpt(cmdLine, "-k", controlRateOverride);
    }
    cmdLine_addStringOpt(cmdLine, "-L", lineInput);
    if (messageLevel != 135 || forceSettings)
      cmdLine_addIntegerOpt(cmdLine, "-m", messageLevel);
    if (enableExpressionOpt)
      cmdLine.push_back("--expression-opt");
    else if (forceSettings)
      cmdLine.push_back("--no-expression-opt");
    cmdLine_addStringOpt(cmdLine, "--env:SADIR+=", sadirPath);
    cmdLine_addStringOpt(cmdLine, "--env:SSDIR+=", ssdirPath);
    cmdLine_addStringOpt(cmdLine, "--env:SFDIR+=", sfdirPath);
    cmdLine_addStringOpt(cmdLine, "--env:INCDIR+=", incdirPath);
    for (int i = 0; i < 10; i++) {
      if (!CsoundGUIMain::isEmptyString(strsets[i])) {
        char  buf[16];
        std::sprintf(&(buf[0]), "--strset%d=", i);
        cmdLine_addStringOpt(cmdLine, &(buf[0]), strsets[i]);
      }
    }
    if (verbose)
      cmdLine.push_back("-v");
    if (enableDither)
      cmdLine.push_back("-Z");
    cmdLine_addStringOpt(cmdLine, "--opcode-lib=", pluginLibs);
    cmdLine_addStringOpt(cmdLine, "-+id_artist=", sndidArtist);
    cmdLine_addStringOpt(cmdLine, "-+id_comment=", sndidComment);
    cmdLine_addStringOpt(cmdLine, "-+id_copyright=", sndidCopyright);
    cmdLine_addStringOpt(cmdLine, "-+id_date=", sndidDate);
    cmdLine_addStringOpt(cmdLine, "-+id_software=", sndidSoftware);
    cmdLine_addStringOpt(cmdLine, "-+id_title=", sndidTitle);
    if (ignoreCSDOptions)
      cmdLine.push_back("-+ignore_csopts=1");
    else if (forceSettings)
      cmdLine.push_back("-+ignore_csopts=0");
    cmdLine_addStringOpt(cmdLine, "-+jack_client=", jackClientName);
    cmdLine_addStringOpt(cmdLine, "-+jack_inportname=", jackInPortName);
    cmdLine_addStringOpt(cmdLine, "-+jack_outportname=", jackOutPortName);
    if ((maxStrLen >= 9 && maxStrLen <= 9999) &&
        (maxStrLen != 255 || forceSettings)) {
      cmdLine_addIntegerOpt(cmdLine, "-+max_str_len=", maxStrLen + 1);
    }
    if (!cmdLine_addStringOpt(cmdLine, "-+mute_tracks=", midiFileMuteTracks)) {
      if (forceSettings)
        cmdLine.push_back("-+mute_tracks=");
    }
    if (rawControllerMode)
      cmdLine.push_back("-+raw_controller_mode=1");
    else if (forceSettings)
      cmdLine.push_back("-+raw_controller_mode=0");
    cmdLine_addStringOpt(cmdLine, "-+rtaudio=", rtAudioModule);
    cmdLine_addStringOpt(cmdLine, "-+rtmidi=", rtMidiModule);
    if (scoreOffsetSeconds > 0.0)
      cmdLine_addDoubleOpt(cmdLine, "-+skip_seconds=", scoreOffsetSeconds);
    else if (forceSettings && scoreOffsetSeconds == 0.0)
      cmdLine.push_back("-+skip_seconds=0");
    if (!CsoundGUIMain::isEmptyString(orcName)) {
      cmdLine.push_back(orcName);
      if (!CsoundGUIMain::isCSDFile(orcName) &&
          !CsoundGUIMain::isEmptyString(scoName))
        cmdLine.push_back(scoName);
    }
}

