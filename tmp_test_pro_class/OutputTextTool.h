#pragma once

#ifndef OUTPUTTEXTTOOL_H
#define OUTPUTTEXTTOOL_H

#include "Platform.h"

class OutputTextTool
{
private :
	static int m_nDotLen;
	static std::string m_sVersion;

public: 
	static void waitText(int sleep_time);
	static void showProgress(int percentage, std::string tailStr = "");
	static void showTextWithVariableDots(std::string str, int count, int total);
};

#endif