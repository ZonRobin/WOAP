#include "../JuceLibraryCode/JuceHeader.h"
#include <shlobj.h>

void startProcess(const File file, bool minimize = false, const String args = String())
{
	const String path = file.getFullPathName();

	if (minimize)
		ShellExecuteW(0, 0, path.toWideCharPointer(), args.toWideCharPointer(), 0, SW_MINIMIZE);
	else
		ShellExecuteW(0, 0, path.toWideCharPointer(), args.toWideCharPointer(), 0, SW_SHOWDEFAULT);
}


int main (int, char*[])
{
	const File currentDir = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getParentDirectory();

	startProcess(currentDir.getChildFile("WebClient").getChildFile("WOAP WebServer.exe"), true);
	startProcess(currentDir.getChildFile("WebProxy").getChildFile("WOAP WebProxy.exe"), true);
	startProcess(currentDir.getChildFile("WOAP Host.exe"));

    return 0;
}