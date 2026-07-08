#include "L2UIMain.h"

#include "../OpenGLImageLoader_Gdiplus.h"

#include "../main.h"

#include "L2UIEditor.h"
#include "L2UIMap.h"
#include "L2UIBusyScreen.h"

L2UIMain::L2UIMain(void)
{
	_isInitialized = false;
}

L2UIMain::~L2UIMain(void)
{
}

void L2UIMain::Init()
{
	mPlatform = new MyGUI::OpenGLPlatform();
	mPlatform->initialise(new OpenGLImageLoader_Gdiplus());

	MyGUI::xml::Document doc;

	if (!doc.open(std::string("resources.xml")))
		doc.getLastError();

	MyGUI::xml::ElementPtr root = doc.getRoot();
	if (root == nullptr || root->getName() != "Paths")
		return;

	std::string mRootMedia;

	MyGUI::xml::ElementEnumerator node = root->getElementEnumerator();
	while (node.next())
	{
		if (node->getName() == "Path")
		{
			bool root = false;
			if (node->findAttribute("root") != "")
			{
				root = MyGUI::utility::parseBool(node->findAttribute("root"));
				if (root) mRootMedia = node->getContent();
			}
			mPlatform->getDataManagerPtr()->addResourceLocation(node->getContent(), false);
		}
	}


	mGUI = new MyGUI::Gui();
	mGUI->initialise();

	MyGUI::PointerManager::getInstance().setVisible(false);

	MyGUI::ResourceManager::getInstance().load("L2EditorLayers.xml");

	_L2Editor = new L2UIEditor();
	_L2Editor->Init();
	_widgets.add(_L2Editor);
	_L2Map = new L2UIMap();
	_L2Map->Init();
	_widgets.add(_L2Map);
	_L2BusyScreen = new L2UIBusyScreen();
	_L2BusyScreen->Init();
	_widgets.add(_L2BusyScreen);

	_isInitialized = true;
}

void L2UIMain::update()
{
	for (int i = 0; i < _widgets.Size(); i++)
	{
		_widgets[i]->update();
	}

	/*
		// Получаем позицию
		auto pos = g_input.getMousePos();

		// Проверяем на валидность (например, если координаты -1 или NaN)
		if (pos.x == -1 || pos.y == -1) {
			// Не передаем данные в MyGUI, если мышь вне окна
			// Или передаем 0, 0, если логика движка это позволяет
		}
		else {

			int _debug = 0;

			MyGUI::InputManager::getInstance().injectMouseMove(pos.x, pos.y, g_input.getMouseScroll());

		}

	*/



	auto pos = g_input.getMousePos();

	// 1. Сначала проверяем валидность данных
	if (pos.x < 0 || pos.y < 0 || std::isnan(pos.x) || std::isnan(pos.y)) {
		// Мышь вне окна или данные битые. 
		// Не передаем событие в MyGUI, чтобы не вызвать исключение.
		// Можно передать (0,0) или просто ничего не делать.
		return; // или continue, если это цикл
	}

	// 2. Только если данные валидны, вызываем инъекцию
	try {
		MyGUI::InputManager::getInstance().injectMouseMove(pos.x, pos.y, g_input.getMouseScroll());
	}
	catch (const MyGUI::Exception& e) {
		// На всякий случай ловим исключение, чтобы понять, что случилось
		// В релизной версии этот блок можно убрать, но для отладки полезен
		OutputDebugStringA(("MyGUI Exception: " + std::string(e.what())).c_str());
	}

	// ORIG
	//MyGUI::InputManager::getInstance().injectMouseMove(g_input.getMousePos().x, g_input.getMousePos().y, g_input.getMouseScroll());

	glPushMatrix();
	glLoadIdentity();
	mPlatform->getRenderManagerPtr()->drawOneFrame();
	glPopMatrix();
}

void L2UIMain::onResize(int width, int height)
{
	if (!_isInitialized)
		return;

	mPlatform->getRenderManagerPtr()->setViewSize(width, height);

	for (int i = 0; i < _widgets.Size(); i++)
	{
		_widgets[i]->onResize(width, height);
	}
}

bool L2UIMain::injectMouseDown(int x, int y, MyGUI::MouseButton btn)
{
	if (MyGUI::InputManager::getInstance().injectMousePress(x, y, btn))
		return true;

	for (int i = 0; i < _widgets.Size(); i++)
	{
		if (_widgets[i]->injectMouseDown(x, y, btn))
			return true;
	}

	return false;
}

bool L2UIMain::injectMouseUp(int x, int y, MyGUI::MouseButton btn)
{
	if (MyGUI::InputManager::getInstance().injectMouseRelease(x, y, btn))
		return true;

	for (int i = 0; i < _widgets.Size(); i++)
	{
		if (_widgets[i]->injectMouseUp(x, y, btn))
			return true;
	}

	return false;
}