#include "pch.h"
#include "Graph.h"
#include "SFMLButton.h"
#include<windows.h>

using namespace System;
using namespace sf;
using namespace std;

int counter = 1;

void CenterText(sf::Text &text, RectangleShape &shape) //Метод для оцентровки текста в прямоугольнике
{
	FloatRect bounds(text.getLocalBounds());
	Vector2f box(shape.getSize());

	text.setOrigin((bounds.width - box.x) / 2 + bounds.left, (bounds.height - box.y) / 2 + 
		bounds.top);
}

//Метод получения значения из диалогового окна
int GetValueFromDialog()
{
	//Создание окна
	RenderWindow dialogWindow(VideoMode(800, 135), "Enter Data");

	string currentText = "";

	Font font;
	font.loadFromFile("CyrilicOld.TTF");

	while (dialogWindow.isOpen())
	{
		dialogWindow.clear(Color(255, 125, 24));

		RectangleShape TextBox(Vector2f(521, 31));

		TextBox.setPosition(120, 10);

		TextBox.setOutlineColor(Color::Black);

		sf::Text text;

		text.setString(currentText.length() == 0 ? "Введите длину пути" : currentText + "_");
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);
		text.setCharacterSize(15);
		text.setPosition(TextBox.getPosition());
		text.setFont(font);

		RectangleShape ExitButton(Vector2f(241, 41));

		ExitButton.setPosition(250, 60);
		ExitButton.setOutlineColor(Color::Black);

		sf::Text OkText;

		OkText.setString("Ok");
		OkText.setFillColor(Color::Black);
		OkText.setOutlineColor(Color::White);
		OkText.setCharacterSize(20);
		OkText.setPosition(ExitButton.getPosition());
		OkText.setFont(font);

		CenterText(OkText, ExitButton);
		CenterText(text, TextBox);

		dialogWindow.draw(TextBox);
		dialogWindow.draw(text);
		dialogWindow.draw(ExitButton);
		dialogWindow.draw(OkText);

		Uint32 uniText;

		Event ev;
		char chr;

		while (dialogWindow.pollEvent(ev))
		{
			switch (ev.type)
			{
			case Event::Closed:
				dialogWindow.close();
				currentText = "";
				break;

			case Event::MouseButtonPressed:

				if (ev.key.code == Mouse::Left)
				{
					Vector2i mousePosition = Mouse::getPosition(dialogWindow);

					if (ExitButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
						dialogWindow.close();
				}

				break;


			case Event::TextEntered:

				uniText = ev.text.unicode;

				//0x8 - backspace, при его использовании удаляем символ из строки
				if (uniText == 0x8 && currentText.length() != 0)
					currentText.erase(currentText.size() - 1);

				//Максимум два символа
				else if (currentText.length() < 2)
				{
					chr = static_cast<wchar_t>(uniText);

					//Не вводим символ в строку, если он не цифра
					if (isdigit(chr))
						currentText += static_cast<wchar_t>(ev.text.unicode);
				}

				break;

			default:
				break;
			}
		}

		dialogWindow.display();
	}

	//Возращаем значение, введеное пользователем

	if (currentText == "")
		return -1;

	return stoi(currentText);
}

void OpenResultWindow(Graph<string>& graph, string& choicedElement) 
{
	vector<string> solution = graph.FloydsAlgotrithm(choicedElement);

	string result = solution[0];

	for (int i = 1; i < solution.size(); i++)
		result += '\n' + solution[i];

	int maxLenght = 0;

	for (int i = 0; i < solution.size(); i++)
		maxLenght = solution[i].length() > maxLenght ? solution[i].length() : maxLenght;

	sf::Text visualText;

	visualText.setFillColor(Color::Black);
	visualText.setOutlineColor(Color::White);

	Font font;
	font.loadFromFile("CyrilicOld.TTF");

	visualText.setFont(font);
	visualText.setPosition(50, 30);
	visualText.setCharacterSize(30);

	RenderWindow ResultWindow(VideoMode(maxLenght * 35,
		solution.size() * 100), "Result!");

	visualText.setString(result);

	while (ResultWindow.isOpen())
	{
		ResultWindow.clear(Color(255, 125, 24));

		Event ev;

		while (ResultWindow.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
				ResultWindow.close();
		}

		ResultWindow.draw(visualText);

		ResultWindow.display();
	}
}

int main(cli::array<System::String ^> ^args)
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	WorkMode mode = WorkMode::None;

	vector<SFMLButton> Buttons;

	RenderWindow window(VideoMode(800, 600), "MerchantProblem");

	Graph<string> mainGraph; //Основной граф

	vector<string> choosenElements; //Выбранные на графе элементы

	int chosenButtonIndex = -1; //Индекс выбранной кнопки

	srand(time(0));

	while (window.isOpen())
	{
		window.clear(Color(255, 125, 24));
		Buttons.clear();

		//Визуальный холст для графа
		RectangleShape visualCanvas(Vector2f(631, 651));
		visualCanvas.move(200, 0);
		visualCanvas.setFillColor(Color::Green);
		visualCanvas.setOutlineColor(Color::Black);

		//Действительный холст для графа, рисовать можно только на нем
		RectangleShape functionalCanvas(Vector2f(631 - VertexRadius * 2, 651 - VertexRadius * 2));
		functionalCanvas.setPosition(200 + VertexRadius, 0 + VertexRadius);
		functionalCanvas.setFillColor(Color::Green);
		functionalCanvas.setOutlineColor(Color::Green);

		window.draw(visualCanvas);
		window.draw(functionalCanvas);

		//Подготовка кнопок
		SFMLButton::SetUpButtons(10, 20, 40, Buttons, window, chosenButtonIndex);

		Event ev;

		while (window.pollEvent(ev))
		{
			Vector2i mousePosition;

			switch (ev.type)
			{
			case Event::Closed:
				window.close();
				break;

				//При нажатии кнопки мыши
			case Event::MouseButtonPressed:

				//Реагируем только на левое нажатие
				if (ev.key.code == Mouse::Left) 
				{
					mousePosition = Mouse::getPosition(window);

					//Пытаемся выбрать элемент в графе, если это требуется
					if ((mode == MoveVertex  || mode == MerchantProblem) && choosenElements.size() == 0 ||
						((mode == CreateEdge || mode == DestroyEdge) && choosenElements.size() < 2))
						mainGraph.TrySetNewChoosen(mousePosition, choosenElements);

					int anotheButtonIndex = -1;

					//Пытаемся выбрать новую кнопку
					for (int i = 0; i < 6 && anotheButtonIndex == -1; i++)
					{
						SFMLButton& button = Buttons[i];

						if (button.getShape().getGlobalBounds().contains(mousePosition.x, mousePosition.y))
							anotheButtonIndex = i;
					}

					//Если смогли
					if (anotheButtonIndex != -1)
					{
						//Меняем кнопку
						chosenButtonIndex = anotheButtonIndex;
						mode = Buttons[chosenButtonIndex].GetWorkMode();
						choosenElements.clear();
					}
					else //Иначе
					{
						string content;

						//Выполняем текущий режим работы
						switch (mode)
						{
						case CreateVertex: //Добавить вершину

							if (functionalCanvas.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
							{
								content = "Г" + to_string(counter++); //Вершина генерируется сама
								mainGraph.AddVertex(content, mousePosition);
							}
							
							break;
						case DestroyVertex: //Удалиить вершину

							mainGraph.TryRemoveVertexByCoordinates(mousePosition);
							break;

						case MoveVertex: //Подвинуть вершину

							if (choosenElements.size() != 0 && 
								functionalCanvas.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
								mainGraph.MoveVertex(choosenElements[0], mousePosition);

							break;

						case CreateEdge: //Добавить ребро

							if (choosenElements.size() == 2)
							{
								mainGraph.AddEdge(choosenElements[0], choosenElements[1], GetValueFromDialog());
								choosenElements.clear();
							}

							break;
						case DestroyEdge: //Уничтожить ребро

							if (choosenElements.size() == 2)
							{
								mainGraph.RemoveEdge(choosenElements[0], choosenElements[1]);
								choosenElements.clear();
							}

							break;
						case MerchantProblem: //Решить задачу коммивояжера

							if (choosenElements.size() == 0)
								break;

							OpenResultWindow(mainGraph, choosenElements[0]);

							choosenElements.clear();

							break;
						case None: //Ничего не делаем, если режим не задан
							break;
						default:
							break;
						}
					}
				}
				break;

			default:
				break;
			}	
		}

		//Отрисовка графа
		mainGraph.DrawGraph(window, choosenElements);

		window.display();
	}
}
