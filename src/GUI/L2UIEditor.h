#pragma once

#include "L2UIBaseWidget.h"

class L2UIEditor : public L2UIBaseWidget
{
public:
	virtual void Init();

	virtual void update();

	virtual bool injectMouseDown(int x, int y, MyGUI::MouseButton btn);
	virtual bool injectMouseUp(int x, int y, MyGUI::MouseButton btn);

	void onTopMenu_Main_ShowMap(MyGUI::Widget* sender);
	void onBtn1MouseClick(MyGUI::Widget* sender);
	void onShowBspMouseClick(MyGUI::Widget* sender);

	// ИСПРАВЛЕНИЕ: Новый метод-делегат обработки выбора карты из списка
	void onComboMapAccept(MyGUI::ComboBox* _sender, size_t _index);

	std::string currentLoadedMapName = "None";

protected:
	MyGUI::MenuBarPtr ui_topMenu;
	MyGUI::ImageBox* ui_leftPanel;
	MyGUI::Button* ui_sceneShowBsp;

	// ИСПРАВЛЕНИЕ: Указатель на создаваемый выпадающий список
	MyGUI::ComboBox* ui_comboMaps;

public:
	L2UIEditor();
	virtual ~L2UIEditor();
};

