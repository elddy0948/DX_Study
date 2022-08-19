#pragma once

#include "BaseWindow.h"

namespace base
{
	class MainWindow : public BaseWindow<MainWindow>
	{
	public:
		MainWindow();
		PCWSTR ClassName() const;
		LRESULT HandleMessage(
			UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}