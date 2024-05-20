#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>

#define PI 3.14159265 // Константа, выражающая собой число Pi

#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

const int VertexRadius = 20; //Радиус вершины графа

const int startMinValue = 10000; //Недостижимый минимум

struct ZeroElement //Структура, олицетворяющая собой нулевой элемент
{
	int Row; //Ряд элемента
	int Column; //Столбец элемента
	int Value; //Оценка элемента

	ZeroElement() {};

	ZeroElement(int row, int column, int value)
	{
		Row = row;
		Column = column;
		Value = value;
	}

	void operator=(ZeroElement &other) 
	{
		Row = other.Row;
		Column = other.Column;
		Value = other.Value;
	}
};

template<typename T>
struct TurnBackPoint //Структура точки возврата
{
	vector<vector<int>> Matrix; //Матрица графа на тот момент

	ZeroElement zero_element; //Нулевой элемент, который будем использовать

	map<T, T> RecordedPath; //Путь на тот момент

	vector<pair<T, T>> Buffer; //Буффер на тот момент

	int Counter; //Счетчик на тот момент

	TurnBackPoint() {}

	TurnBackPoint(vector<vector<int>> &matrix, ZeroElement &zeroElm, map<T, T> &recordedPath, int counter, vector<pair<T, T>>& buffer) 
	{
		Matrix = matrix;
		zero_element = zeroElm;
		RecordedPath = recordedPath;
		Counter = counter;
		Buffer = buffer;
	}
};

template<typename T>
struct GraphElement //Структура элемента графа
{
	CircleShape visualVertex; //Визуальная часть элемента графа
	
	T Data; //Данные
};

template <typename T>
class Graph
{
public:

	void DrawGraph(RenderWindow &window, vector<T> &choosenElements) //Метод рисования графа
	{

		for (int i = 0; i < VertexList.size(); i++)
			DrawVertex(window, VertexList[i], choosenElements); //Рисуем вершины

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
				DrawEdge(window, VertexList[i], VertexList[j], AdjMatrix[i][j]); //Рисуем грани
		}
	}

	void TryRemoveVertexByCoordinates(Vector2i &coords) //Метод удаления вершины по координатам
	{
		T item;
		bool flag = false;

		for (int i = 0; i < VertexList.size() && !flag; i++) //Находим вершину по координатам
		{
			flag = VertexList[i].visualVertex.getGlobalBounds().contains(coords.x, coords.y);

			if (flag)
				item = VertexList[i].Data;
		}

		if (flag) //Удаляем, если нашли
			RemoveVertex(item);
	}

	void AddVertex(T vertex, Vector2i position) //Метод добавления вершины, принимает данные и позицию
	{
		int oldSize = VertexList.size();

		GraphElement<T> elm;
		
		//Задавания данных новому элементу графа
		elm.Data = vertex;
		elm.visualVertex.setRadius(VertexRadius);
		elm.visualVertex.setPosition(position.x - VertexRadius, position.y - VertexRadius);
		elm.visualVertex.setOutlineColor(Color::Black);
		elm.visualVertex.setFillColor(Color::White);
		elm.visualVertex.setOutlineThickness(2);

		//Добавление
		VertexList.push_back(elm);	
		
		int currentSize = VertexList.size();

		vector<vector<int>> Buffer = AdjMatrix;

		AdjMatrix.clear();

		//Создание колонци и строки для нового элемента графа
		AdjMatrix = vector<vector<int>>(currentSize, vector<int>(currentSize));

		for (int i = 0; i < oldSize; i++)
		{
			for (int j = 0; j < oldSize; j++)
				AdjMatrix[i][j] = Buffer[i][j];
		}
			
		for (int i = oldSize; i < currentSize; i++)
		{
			for (int j = oldSize; j < currentSize; j++)
				AdjMatrix[i][j] = 0;
		}
	}

	void TrySetNewChoosen(Vector2i coords, vector<T> &listChoosen) //По координатам пытаемся определить выбранный пользователем элемент
	{
		bool flag = false;

		T choosenData;

		for (int i = 0; i < VertexList.size() && !flag; i++) 
		{
			flag = VertexList[i].visualVertex.getGlobalBounds().contains(coords.x, coords.y);

			if (flag)
				choosenData = VertexList[i].Data;
		}
			

		if (!flag)
			return;

		flag = false;

		//Если нашли, то проверяем, не выбран ли уже элемент
		for (int i = 0; i < listChoosen.size() && !flag; i++)
			flag = listChoosen[i] == choosenData;

		if (flag)
			return;

		//Если не выбран, то выбираем
		listChoosen.push_back(choosenData);
	}

	vector<string> FloydsAlgotrithm(T startVertex) 
	{
		vector<vector<int>> BufferMatrix = vector<vector<int>>
			(AdjMatrix.size(), vector<int>(AdjMatrix.size()));

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				if (i == j)
					BufferMatrix[i][j] = 0;

				else if (AdjMatrix[i][j] == 0 || AdjMatrix[i][j] == -1)
					BufferMatrix[i][j] = startMinValue;

				else
					BufferMatrix[i][j] = AdjMatrix[i][j];
			}
		}

		vector<vector<int>> resultMatrix = vector<vector<int>>
			(AdjMatrix.size(), vector<int>(AdjMatrix.size()));

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				if (AdjMatrix[i][j] == 0 || AdjMatrix[i][j] == -1)
					resultMatrix[i][j] = -1;
				else
					resultMatrix[i][j] = j;
			}
		}

		for (int i = 0; i < VertexList.size(); i++)
		{
			for (int u = 0; u < VertexList.size(); u++)
			{
				for (int v = 0; v < VertexList.size(); v++)
				{
					if (BufferMatrix[u][i] + BufferMatrix[i][v] < BufferMatrix[u][v]) 
					{
						BufferMatrix[u][v] = BufferMatrix[u][i] + BufferMatrix[i][v];

						resultMatrix[u][v] = resultMatrix[u][i];
					}
				}
			}
		}

		vector<string> result;

		for (int i = 0; i < VertexList.size(); i++) 
		{
			if (startVertex == VertexList[i].Data)
				continue;

			result.push_back(GetShortesPath(startVertex, VertexList[i].Data, 
				resultMatrix, BufferMatrix));
		}

		return result;
	}

	string GetShortesPath(T vertex1, T vertex2, vector<vector<int>>& SolutionMatrix,
		vector<vector<int>>& BufferMatrix)
	{
		string result;
		result = "From " + vertex1 + " to " + vertex2 + ": ";

		if (BufferMatrix[GetVertexIndex(vertex1)][GetVertexIndex(vertex2)] == startMinValue)
			result += "No path found";
		else
		{
			T temp = vertex1;

			while (temp != vertex2)
			{
				result += temp + "->";

				temp = VertexList[SolutionMatrix[GetVertexIndex(temp)]
					[GetVertexIndex(vertex2)]].Data;
			}

			result += vertex2;
		}

		return result;
	}

	//Метод сдвига вершины
	void MoveVertex(T data, Vector2i position) 
	{
		GraphElement<T>* element = nullptr;

		//Находим элемент по заданым данным
		for (int i = 0; i < VertexList.size() && element == nullptr; i++)
		{
			if (VertexList[i].Data == data)
				element = &VertexList[i];
		}

		//Сдивгаем
		element->visualVertex.setPosition(position.x - VertexRadius, position.y - VertexRadius);
	}

	//Метод проверки возможности решения коммивояжера
	bool CanSolveMerchantProblem()
	{
		if (VertexList.size() <= 1)
			return false;

		bool result = true;

		//Условие - граф полный (пути есть между всеми городами)
		for (int i = 0; i < AdjMatrix.size() && result; i++)
		{
			for (int j = 0; j < AdjMatrix.size() && result; j++)
				result = i == j || (AdjMatrix[i][j] != 0 && AdjMatrix[i][j] != -1);
		}

		return result;
	}

	//Возвращает кол-во вершин
	int GetVertexCount()
	{
		return VertexList.size();
	}

	//Метод для решения задачи коммивояжера
	map<T, T> SolveMerchantProblem()
	{
		vector<pair<T, T>> buffer;
		map<T, T> result;

		vector<vector<int>> BufferMatrix = vector<vector<int>>
			(AdjMatrix.size(), vector<int>(AdjMatrix.size()));

		//Создаем буфферную матрицы
		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
				BufferMatrix[i][j] = i == j ? -1 : AdjMatrix[i][j];
		}

		vector<TurnBackPoint<T>> TurnBackPoints;

		cout << "Starting table: " << endl << endl;

		//Печатаем в консоль изначальную матрицу
		PrintTable(BufferMatrix);

		//Заходим в цикл
		for (int v = 0; v < VertexList.size(); v++)
		{
			cout << "----------------------" << endl << endl;

			//Граф на данный момент
			PrintTable(BufferMatrix);

			vector<int> MinElementsOfRows;

			cout << endl <<"Row minimum elements:" << endl;

			//Находим минимум для каждой строки
			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				int minValue = startMinValue;

				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[i][j] >= 0 && minValue > BufferMatrix[i][j])
						minValue = BufferMatrix[i][j];
				}

				cout << i << " : " << minValue << endl;
				MinElementsOfRows.push_back(minValue);
			}

			cout << "Matrix after row reduction:" << endl << endl;;

			//Проводим редукцию
			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[i][j] > 0)
						BufferMatrix[i][j] -= MinElementsOfRows[i];
				}		
			}

			PrintTable(BufferMatrix);

			vector<int> MinElemntsOfColumns;

			cout << endl << endl << "Minimum column elements:" << endl;

			//Находим минимумы по столбцам
			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				int minValue = startMinValue;

				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[j][i] >= 0 && minValue > BufferMatrix[j][i])
						minValue = BufferMatrix[j][i];
				}

				cout << i << " : " << minValue << endl;

				MinElemntsOfColumns.push_back(minValue);
			}

			//Проводим редукцию
			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)

					if (BufferMatrix[j][i] > 0)
						BufferMatrix[j][i] -= MinElemntsOfColumns[i];
			}

			cout << endl << "Matrix after second reduction: " << endl << endl;

			PrintTable(BufferMatrix);

			vector<ZeroElement> ZeroElements;

			//Находим оценку всем нулевым элементам
			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[i][j] == 0)
					{
						int minRow = startMinValue;
						int minColumn = startMinValue;

						for (int uj = 0; uj < BufferMatrix.size(); uj++)
						{
							if (BufferMatrix[i][uj] >= 0 && j != uj)
							{
								if (minRow > BufferMatrix[i][uj])
									minRow = BufferMatrix[i][uj];
							}	
						}

						for (int ui = 0; ui < BufferMatrix.size(); ui++)
						{
							if (BufferMatrix[ui][j] >= 0 && ui != i)
							{
								if (minColumn > BufferMatrix[ui][j])
									minColumn = BufferMatrix[ui][j];
							}
						}

						ZeroElement element(i, j, minColumn + minRow);
						ZeroElements.push_back(element);
					}
				}
			}

			//Если не нашли нулевых элементов вообще
			if (ZeroElements.size() == 0)
			{
				cout << "No more zero elements, finding obvious routes" << endl;

				for (int i = 0; i < VertexList.size(); i++)
				{
					bool flag = false;

					//Находим очевидные решения, которые потерялись в процессе редукции
					if (result.find(VertexList[i].Data) == result.end())
					{
						for (int j = 0; j < VertexList.size() && !flag; j++)
						{
							if (i == j)
								continue;

							bool does_exist = false;

							for (auto& elm : result)
							{
								if (elm.second == VertexList[j].Data)
								{
									does_exist = true;
									break;
								}
							}

							flag = !does_exist;

							if (flag)
								result[VertexList[i].Data] = VertexList[j].Data;
						}
					}
				}
			}
			else //Иначе
			{
				int maxValue = -1;
				vector<ZeroElement> maxElements;

				cout << "Zero elements:" << endl;

				//Печатаем все нулевые элементы в консоль
				for (int i = 0; i < ZeroElements.size(); i++)
					cout << ZeroElements[i].Row << " " << ZeroElements[i].Column << " " << ZeroElements[i].Value << endl;

				//Находим нулевые элементы с максимальным значением
				for (int i = 0; i < ZeroElements.size(); i++)
				{
					if (maxValue < ZeroElements[i].Value)
					{
						maxElements.clear();

						maxElements.push_back(ZeroElements[i]);

						maxValue = ZeroElements[i].Value;
					}
					else if (maxValue == ZeroElements[i].Value)
						maxElements.push_back(ZeroElements[i]);
				}

				if (maxElements.size() > 1) //Если их больше одного
				{
					cout << "More than one max value zero elements found, creating a turn back point for each but not the first one!" << endl;

					//Создаем точку возврата
					for (int i = 1; i < maxElements.size(); i++)
						TurnBackPoints.push_back(TurnBackPoint<T>(BufferMatrix, maxElements[i], result, v, buffer));
				}

				//Выбираем первый из найденых нулевых элементов
				ZeroElement maxElement = maxElements[0];

				pair<T, T> to_add;

				to_add.first = VertexList[maxElement.Row].Data;
				to_add.second = VertexList[maxElement.Column].Data;

				//Закрываем элементы, которые могут привести к созданию неправильного цикла
				for (int i = 0; i < buffer.size(); i++)
				{
					if (to_add.first == buffer[i].second)
						BufferMatrix[GetVertexIndex(to_add.second)][GetVertexIndex(buffer[i].first)] = -1;
				}

				//Добавляем в буффер
				buffer.push_back(to_add);

				cout << "Result pair: " << to_add.first << " " << to_add.second << endl;

				//Добавляем в итоговый маршрут
				result[VertexList[maxElement.Row].Data] = VertexList[maxElement.Column].Data;

				//Закрываем элементы в строке найденного элемента
				for (int i = 0; i < BufferMatrix.size(); i++)
					BufferMatrix[maxElement.Row][i] = -1;

				//В столбце
				for (int i = 0; i < BufferMatrix.size(); i++)
					BufferMatrix[i][maxElement.Column] = -1;

				//Закрываем обратный путь
				BufferMatrix[maxElement.Row][maxElement.Column] = -1;
				BufferMatrix[maxElement.Column][maxElement.Row] = -1;

				cout << "Matrix after full reduction: " << endl << endl;

				PrintTable(BufferMatrix);
			}

			if (v == VertexList.size() - 1) //Если мы находимся на псоледней итерации цикла
			{
				if (CheckInvalid(result)) //Если путь плох
				{
					cout << "Path is found but deemed bad, returning to one of the turn back points!" << endl << endl;

					//Берем последнюю точку возврата
					TurnBackPoint<T>& point = TurnBackPoints[TurnBackPoints.size() - 1];

					//Получаем из нее все данные
					BufferMatrix = point.Matrix;
					result = point.RecordedPath;
					buffer = point.Buffer;
					ZeroElement chosenElement = point.zero_element;
					v = point.Counter;

					pair<T, T> to_add2;

					//Делаем абсолютно такие же действия, как если бы обычным путем нашли бы нулевой из точки

					to_add2.first = VertexList[chosenElement.Row].Data;
					to_add2.second = VertexList[chosenElement.Column].Data;

					for (int i = 0; i < buffer.size(); i++)
					{
						if (to_add2.first == buffer[i].second)
							BufferMatrix[GetVertexIndex(to_add2.second)][GetVertexIndex(buffer[i].first)] = -1;
					}

					buffer.push_back(to_add2);

					cout << "Result pair: " << to_add2.first << " " << to_add2.second << endl;

					result[VertexList[chosenElement.Row].Data] = VertexList[chosenElement.Column].Data;

					for (int i = 0; i < BufferMatrix.size(); i++)
						BufferMatrix[chosenElement.Row][i] = -1;


					for (int i = 0; i < BufferMatrix.size(); i++)
						BufferMatrix[i][chosenElement.Column] = -1;

					BufferMatrix[chosenElement.Row][chosenElement.Column] = -1;
					BufferMatrix[chosenElement.Column][chosenElement.Row] = -1;

					TurnBackPoints.pop_back(); //Удаляем точку возврата

					cout << "Matrix after full reduction: " << endl << endl;

					PrintTable(BufferMatrix);
				}
				else //Иначе просто заканчиваем цикл
					cout << "Path is found and deemed good!" << endl;
			}	
		}

		return result; //Возвращаем результат
	}

	void AddEdge(T vertex1, T vertex2, int weight) //Метод добавления ребра в граф
	{
		if (weight <= 0)
			return;

		int vertex1Index = GetVertexIndex(vertex1);
		int vertex2Index = GetVertexIndex(vertex2);

		AdjMatrix[vertex1Index][vertex2Index] = weight;

		if (AdjMatrix[vertex2Index][vertex1Index] == 0)
			AdjMatrix[vertex2Index][vertex1Index] = -1;
	}

	void RemoveEdge(T vertex1, T vertex2) //Метод удаления из графа ребра 
	{
		int vertex1Index = GetVertexIndex(vertex1);
		int vertex2Index = GetVertexIndex(vertex2);

		if (AdjMatrix[vertex2Index][vertex1Index] != -1)
			AdjMatrix[vertex1Index][vertex2Index] = -1;
		else
		{
			AdjMatrix[vertex1Index][vertex2Index] = 0;
			AdjMatrix[vertex2Index][vertex1Index] = 0;
		}		
	}

	int GetWeight(T vertex1, T vertex2) //Метод получения веса ребра между двумя вершинами
	{
		int v1index = GetVertexIndex(vertex1);
		int v2index = GetVertexIndex(vertex2);

		return AdjMatrix[v1index][v2index];;
	}

	void Clear() //Метод очистки графа
	{
		VertexList.clear();
		AdjMatrix.clear();
	}

private:

	//Список элементов графа
	vector<GraphElement<T>> VertexList;

	//Матрица смежности
	vector<vector<int>> AdjMatrix;

	bool CheckInvalid(map<T, T>& path)
	{
		vector<T> metPoints;

		T current = VertexList[0].Data;

		bool result = false;

		//Проходим по всему маршруту
		for (int i = 0; i < VertexList.size() - 1 && !result; i++)
		{
			//Проверяем, встречался ли уже этот город
			for (int j = 0; j < metPoints.size() && !result; j++)
				result = metPoints[j] == current;

			//Переход к следующему
			metPoints.push_back(current);
			current = path[current];
		}

		//Возрат флага
		return result;
	}

	//Метод печати матрицы смежности в консоль
	void PrintTable(vector<vector<int>> to_print)
	{
		//Получаем самый длинный элемент в таблице, включая элементы графа
		int greatestLenght = GetWidestLenght();

		for (int i = 0; i < greatestLenght; i++)
			cout << "_";

		cout << "|";

		for (int i = 0; i < VertexList.size(); i++)
		{
			PrintWithSpaces(VertexList[i].Data, greatestLenght);

			cout << "|";
		}

		PrintDivideLine(VertexList.size() * greatestLenght * 2);

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			PrintWithSpaces(VertexList[i].Data, greatestLenght);

			cout << "|";

			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				if (to_print[i][j] < 0)
					PrintWithSpaces("m", greatestLenght);
				else
					PrintWithSpaces(to_string(to_print[i][j]), greatestLenght);

				cout << "|";
			}

			PrintDivideLine(VertexList.size() * greatestLenght * 2);
		}
	}

	void RemoveVertex(T vertex) //Метод удаления вершины из графа
	{
		int vIndex = GetVertexIndex(vertex);

		//Буфер вершин
		vector<GraphElement<T>> VertexBuffer;

		//Переносим в него все вершины, кроме удаляемой
		for (int i = 0; i < VertexList.size(); i++)
		{
			if (VertexList[i].Data != vertex)
				VertexBuffer.push_back(VertexList[i]);
		}

		int oldSize = VertexList.size();

		//Задаем основному списку новые элементы
		VertexList.clear();
		VertexList = VertexBuffer;

		int currentSize = VertexList.size();

		//Буфер ребер
		vector<vector<int>> EdgesBuffer = AdjMatrix;

		AdjMatrix.clear();

		AdjMatrix = vector<vector<int>>(currentSize, vector<int>(currentSize));

		int ColumnOffset = 0;

		//Переносим в матрицу основную матрицу нужные значения
		for (int i = 0; i < currentSize; i++)
		{
			if (i == vIndex)
				ColumnOffset++;

			int RowOffset = 0;

			for (int j = 0; j < currentSize; j++)
			{
				if (j == vIndex)
					RowOffset++;

				AdjMatrix[i][j] = EdgesBuffer[i + ColumnOffset][j + RowOffset];
			}
		}
	}

	//Метод рисования вершины
	void DrawVertex(RenderWindow &window, GraphElement<T> &Vertex, vector<T> &choosenElements) 
	{
		sf::Text text;
		Font font;

		font.loadFromFile("CyrilicOld.TTF");

		//Настройка текста
		text.setString(Vertex.Data);
		text.setFont(font);
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);
		text.setCharacterSize(VertexRadius);

		//Отцентровка текста
		FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		text.setPosition(Vertex.visualVertex.getPosition() + Vector2f(VertexRadius, VertexRadius));

		bool flag = false;

		for (int i = 0; i < choosenElements.size() && !flag; i++)
			flag = choosenElements[i] == Vertex.Data;

		Vertex.visualVertex.setOutlineColor(flag ? Color::Magenta : Color::Black);

		window.draw(Vertex.visualVertex);
		window.draw(text);
	}
	
	//Метод рисования ребра
	void DrawEdge(RenderWindow& window, GraphElement<T> &Vertex1, GraphElement<T> &Vertex2, int weight)
	{
		if (weight == 0 || weight == -1)
			return;

		//Расчет точки соприкосновения с линией для первого элемента
		Vector2f boundaryForVertex1 =
			CalculateBoundaryPoint(Vertex2.visualVertex.getPosition(), Vertex1.visualVertex.getPosition(), 22)
			+ Vector2f(VertexRadius, VertexRadius);

		//Для второго
		Vector2f boundaryForVertex2 =
			CalculateBoundaryPoint(Vertex1.visualVertex.getPosition(), Vertex2.visualVertex.getPosition(), 22)
			+ Vector2f(VertexRadius, VertexRadius);

		VertexArray Edge(Lines, 2);

		//Рисуем линии
		Edge[0] = boundaryForVertex1;
		Edge[1] = boundaryForVertex2;

		Edge[0].color = Color::Black;
		Edge[1].color = Color::Black;

		//Расчитываем местопложение содержимого ребра
		Vector2f weightTextPosition = CalculateBoundaryPoint(boundaryForVertex1, boundaryForVertex2, 
			sqrt(pow(boundaryForVertex2.x - boundaryForVertex1.x, 2) + pow(boundaryForVertex2.y - boundaryForVertex1.y, 2)) 
			/ 4);

		Font font;

		//Загрузка шрифта
		font.loadFromFile("CyrilicOld.TTF");

		//Настройка текста
		sf::Text text;
		text.setFont(font);
		text.setString(to_string(weight));
		text.setCharacterSize(15);
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);

		FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		text.setPosition(weightTextPosition);

		//Стрелка ребра
		VertexArray triangleArrow(Triangles, 3);

		//Получение угла, под которым должна быть повернута стрелка
		double angle = GetAngle(boundaryForVertex2, boundaryForVertex1, Vector2f(boundaryForVertex2.x, boundaryForVertex2.y + 20));

		if (boundaryForVertex1.x > boundaryForVertex2.x)
			angle *= -1;

		Transform rotation;
		rotation.rotate(angle, boundaryForVertex2);

		//Точки B и C треугольника
		Vector2f pointB = Vector2f(boundaryForVertex2.x - 10, boundaryForVertex2.y + 12);
		Vector2f pointC = Vector2f(boundaryForVertex2.x + 10, boundaryForVertex2.y + 12);

		//Рисуем триугольную стрелку

		triangleArrow[0] = boundaryForVertex2;
		triangleArrow[1] = rotation.transformPoint(pointB);
		triangleArrow[2] = rotation.transformPoint(pointC);

		triangleArrow[0].color = Color::Black;
		triangleArrow[1].color = Color::Black;
		triangleArrow[2].color = Color::Black;

		window.draw(text);
		window.draw(triangleArrow);
		window.draw(Edge);
	}

	double GetAngle(Vector2f &A, Vector2f &B, Vector2f &C) //Метод рассчета угла
	{
		double AB = GetSideLenght(A, B); //Длина стороны AB

		double AC = GetSideLenght(A, C); //Длина стороны AC

		double BC = GetSideLenght(B, C); //Длина стороны BC

		return acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC)) * 180/PI; //Возвращаем угл
	}

	double GetSideLenght(Vector2f &point1, Vector2f &point2) //Метод нахождения длины между двумя точками
	{
		return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
	}

	//Метод для рассчета точки соприкосновения между вершиной и ребром
	Vector2f CalculateBoundaryPoint(Vector2f point1, Vector2f point2, double minus)
	{
		double Lenght = GetSideLenght(point1, point2) - minus;

		double a = atan2(point2.y - point1.y, point2.x - point1.x);

		return Vector2f(point1.x + Lenght * cos(a), point1.y + Lenght * sin(a));
	}

	//Метод для получния индекса вершины
	int GetVertexIndex(T vertex)
	{
		int result = -1;

		for (int i = 0; i < VertexList.size() && result == -1; i++)
		{
			if (VertexList[i].Data == vertex)
				result = i;
		}

		return result;
	}

	//Метод для нахождения самого длинного элемента в матрице графа
	int GetWidestLenght()
	{
		int result = 0;

		for (int i = 0; i < VertexList.size(); i++)
		{
			string element = VertexList[i].Data;

			if (result < element.length())
				result = element.length();
		}

		for (int i = 0; i < VertexList.size(); i++)
		{
			for (int j = 0; j < VertexList.size(); j++)
			{
				string element = to_string(AdjMatrix[i][j]);

				if (result < element.length())
					result = element.length();
			}
		}

		return result;
	}

	//Метод печати элемент в консоль
	void PrintWithSpaces(string element, int maxLenght)
	{
		cout << element;

		for (int u = 0; u < maxLenght - element.length(); u++)
			cout << " ";
	}

	//Метод печати разделяющей линии
	void PrintDivideLine(int Lenght)
	{
		cout << endl;

		for (int i = 0; i < Lenght * 2; i++)
			cout << "-";

		cout << endl;
	}
};

