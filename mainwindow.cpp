#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	DebugMode_ = QCoreApplication::arguments().contains(QString("dbg"));
	MinesShown_ = false;

    Translator_ = new QTranslator(this);
	const QStringList uiLanguages = QLocale::system().uiLanguages();
	for (const QString &locale : uiLanguages)
	{
		if (Translator_->load(":/i18n/minesweeper_" + QLocale(locale).name()))
		{
			qApp->installTranslator(Translator_);
			break;
		}
	}

	setWindowTitle(tr("Minesweeper"));
	setMaximumSize(1000, 1000);

	Rows_ = 10;
	Columns_ = 10;
	Mines_ = 10;
	FlagsRaised_ = 0;

	createToolBar();

	createInterface();

	QString filename = QCoreApplication::applicationDirPath() + "/game_save.ini";
	if (QFile::exists(filename))
	{
		loadGameState(filename);
	}
	else
	{
		newGame();
	}
}

MainWindow::~MainWindow() {}

void MainWindow::createToolBar()
{
	QToolBar *ToolBar = new QToolBar(this);
	addToolBar(Qt::TopToolBarArea, ToolBar);
	ToolBar->setMovable(false);

	QMenu *GameMenu = new QMenu(tr("Game"), this);

	QAction *newGame = new QAction(tr("New game"), this);
	QAction *customGame = new QAction(tr("Settings"), this);
	QAction *quitGame = new QAction(tr("Leave game"), this);

	GameMenu->addAction(newGame);
	GameMenu->addAction(customGame);
	GameMenu->addSeparator();
	GameMenu->addAction(quitGame);

	QToolButton *GameMenuButton = new QToolButton(this);
	QFont Font = GameMenuButton->font();
	Font.setPointSize(14);
	GameMenuButton->setFont(Font);
	GameMenuButton->setText(tr("Game"));
	GameMenuButton->setMenu(GameMenu);
	GameMenuButton->setPopupMode(QToolButton::InstantPopup);
	ToolBar->addWidget(GameMenuButton);

	if (DebugMode_)
	{
		QAction *peek = new QAction(tr("Peek"), this);
		Font = peek->font();
		Font.setPointSize(14);
		peek->setFont(Font);
		peek->setText(tr("Peek"));
		ToolBar->addAction(peek);

		connect(peek, &QAction::triggered, this, &MainWindow::peek);
	}

	QAction *invertClick = new QAction(tr("Left-handed"), this);
	Font = invertClick->font();
	Font.setPointSize(14);
	invertClick->setFont(Font);
	invertClick->setText(tr("Left-handed"));
	ToolBar->addAction(invertClick);

	QMenu *LanguageMenu = new QMenu(tr("Language"), this);

	QAction *setEng = new QAction("English", this);
	QAction *setEs = new QAction("Español", this);
	QAction *setRu = new QAction("Русский", this);

	LanguageMenu->addAction(setEng);
	LanguageMenu->addAction(setEs);
	LanguageMenu->addAction(setRu);

	QToolButton *LanguageMenuButton = new QToolButton(this);
	Font = LanguageMenuButton->font();
	Font.setPointSize(14);
	LanguageMenuButton->setFont(Font);
	LanguageMenuButton->setText(tr("Language"));
	LanguageMenuButton->setMenu(LanguageMenu);
	LanguageMenuButton->setPopupMode(QToolButton::InstantPopup);
	ToolBar->addWidget(LanguageMenuButton);

	CounterLabel_ = new QLabel(tr("Mines left: ") + QString::number(Mines_ - FlagsRaised_), this);
	Font = CounterLabel_->font();
	Font.setPointSize(14);
	CounterLabel_->setFont(Font);

    QWidget *leftSpace = new QWidget(this);
	leftSpace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	ToolBar->addWidget(leftSpace);

	ToolBar->addWidget(CounterLabel_);

    QWidget *rightSpace = new QWidget(this);
	rightSpace->setMinimumWidth(20);
	ToolBar->addWidget(rightSpace);

	connect(newGame, &QAction::triggered, this, &MainWindow::newGame);
	connect(customGame, &QAction::triggered, this, &MainWindow::customGame);
	connect(quitGame, &QAction::triggered, this, &MainWindow::exitGame);
	connect(invertClick, &QAction::triggered, this, &MainWindow::invertClick);
	connect(setEng, &QAction::triggered, this, &MainWindow::setEng);
	connect(setEs, &QAction::triggered, this, &MainWindow::setEs);
	connect(setRu, &QAction::triggered, this, &MainWindow::setRu);
}

void MainWindow::createInterface()
{
	QWidget *CentralWidget = new QWidget(this);
	setCentralWidget(CentralWidget);

	QVBoxLayout *FieldLayout = new QVBoxLayout(CentralWidget);

    Grid_ = new QGridLayout(this);
	Grid_->setSpacing(0);
	Grid_->setContentsMargins(0, 0, 0, 0);

	FieldLayout->addLayout(Grid_);
	FieldLayout->setAlignment(Grid_, Qt::AlignCenter);
	FieldLayout->setContentsMargins(5, 5, 5, 5);

	CentralWidget->setLayout(FieldLayout);

	newGame();
}

void MainWindow::createField()
{
	QLayoutItem *Item;
	while ((Item = Grid_->takeAt(0)) != nullptr)
	{
		delete Item->widget();
		delete Item;
	}

	MineField_.resize(Rows_);
	HasMine_.resize(Rows_);
	MineCount_.resize(Rows_);

	for (int i = 0; i < Rows_; ++i)
	{
		MineField_[i].resize(Columns_);
		HasMine_[i].resize(Columns_);
		MineCount_[i].resize(Columns_);

		for (int j = 0; j < Columns_; ++j)
		{
			MultiButton *Button = MineField_[i][j] = new MultiButton(this, &Inversion_);

			Button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			Button->setMinimumSize(40, 40);
			Button->setStyleSheet("background-color: darkcyan; border: 1px solid black;");

			Button->setProperty("Row", i);
			Button->setProperty("Column", j);
			Button->setProperty("Opened", false);
			Button->setProperty("Flag", false);
			Button->setProperty("Question", false);

			HasMine_[i][j] = false;
			MineCount_[i][j] = 0;

			Grid_->addWidget(Button, i, j);

			connect(Button, &MultiButton::leftClicked, this, &MainWindow::handleLeftClick);
			connect(Button, &MultiButton::rightClicked, this, &MainWindow::handleRightClick);
			connect(Button, &MultiButton::middleClicked, this, &MainWindow::handleMiddleClick);
		}
	}

	CounterLabel_->setText(tr("Mines left: ") + QString::number(Mines_ - FlagsRaised_));
	OpenedCells_ = 0;
	ClosedCells_ = Rows_ * Columns_ - Mines_;
	FirstClick_ = false;
	Inversion_ = false;
}

void MainWindow::placeMines(const int IgnoredRow, const int IgnoredColumn)
{
	int PlacedMines = 0;

	while (PlacedMines < Mines_)
	{
		int Row = QRandomGenerator::global()->bounded(Rows_);
		int Column = QRandomGenerator::global()->bounded(Columns_);

		if (Row == IgnoredRow && Column == IgnoredColumn)
		{
			continue;
		}

		if (!HasMine_[Row][Column])
		{
			HasMine_[Row][Column] = true;
			PlacedMines++;
		}
	}

	for (int i = 0; i < Rows_; ++i)
	{
		for (int j = 0; j < Columns_; ++j)
		{
			if (HasMine_[i][j])
			{
				continue;
			}
			int count = 0;
			for (int x = -1; x <= 1; ++x)
			{
				for (int y = -1; y <= 1; ++y)
				{
					int ni = i + x;
					int nj = j + y;

					if (ni >= 0 && ni < Rows_ && nj >= 0 && nj < Columns_)
					{
						if (HasMine_[ni][nj])
						{
							++count;
						}
					}
				}
			}
			MineCount_[i][j] = count;
		}
	}
}

void MainWindow::openCell(int Row, int Column)
{
	if (Row < 0 || Row >= Rows_ || Column < 0 || Column >= Columns_)
	{
		return;
	}

	MultiButton *Button = MineField_[Row][Column];

	if (Button->property("Opened").toBool())
	{
		return;
	}

	if (Button->property("Flag").toBool())
	{
		return;
	}

	int count = MineCount_[Row][Column];
	Button->setText(count > 0 ? QString::number(count) : "");
	Button->setProperty("Opened", true);
	Button->setStyleSheet("color: darkcyan; background-color: lightgray; border: 1px solid black;");
	OpenedCells_++;

	if (OpenedCells_ == ClosedCells_)
	{
		QMessageBox::information(this, tr("Congratulations"), tr("You won!"));
		for (int i = 0; i < Rows_; ++i)
		{
			for (int j = 0; j < Columns_; ++j)
			{
				MineField_[i][j]->setEnabled(false);
			}
		}
		return;
	}

	if (count == 0)
	{
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				if (x == 0 && y == 0)
				{
					continue;
				}

				int ni = Row + x;
				int nj = Column + y;

				openCell(ni, nj);
			}
		}
	}
}

void MainWindow::loseGame(QPushButton *Button)
{
	MinesShown_ = false;
	toggleMines();

	Button->setText("X");
	Button->setStyleSheet("color: black; background-color: red; border: 1px solid black;");

	QMessageBox::information(this, tr("Game Over"), tr("You clicked on a mine!"));
	newGame();
}

void MainWindow::toggleMines()
{
	for (int Row = 0; Row < Rows_; ++Row)
	{
		for (int Column = 0; Column < Columns_; ++Column)
		{
			MultiButton *Button = MineField_[Row][Column];

			if (HasMine_[Row][Column])
			{
				if (!MinesShown_)
				{
					Button->setText("X");
					Button->setStyleSheet(
						"QPushButton { color: black; border: 1px solid black;}"
						"QPushButton:disabled { color: white; background-color: black; border: 1px "
						"solid black;}");
					Button->setEnabled(false);
				}
				else
				{
					if (!Button->property("Opened").toBool())
					{
						if (Button->property("Flag").toBool())
						{
							if (Button->property("Question").toBool())
							{
								Button->setText("?");
							}
							else
							{
								Button->setText("|>");
							}
						}
						else
						{
							Button->setText("");
						}

						Button->setStyleSheet("background-color: darkcyan; border: 1px solid black;");
						Button->setEnabled(true);
					}
				}
			}
		}
	}

	MinesShown_ = !MinesShown_;
}

void MainWindow::handleLeftClick()
{
	MultiButton *Button = qobject_cast< MultiButton * >(sender());

	if (Button)
	{
		int Row = Button->property("Row").toInt();
		int Column = Button->property("Column").toInt();

		if (Button->property("Flag").toBool())
		{
			return;
		}

		if (!FirstClick_)
		{
			placeMines(Row, Column);
			FirstClick_ = true;
		}

		if (HasMine_[Row][Column])
		{
			loseGame(Button);
		}
		else
		{
			openCell(Row, Column);
		}
	}
}

void MainWindow::handleRightClick()
{
	MultiButton *Button = qobject_cast< MultiButton * >(sender());

	if (Button)
	{
		if (Button->property("Opened").toBool())
		{
			return;
		}

		if (Button->property("Flag").toBool())
		{
			if (Button->property("Question").toBool())
			{
				Button->setText("");
				Button->setProperty("Flag", false);
				Button->setProperty("Question", false);
			}
			else
			{
				Button->setText("?");
				Button->setProperty("Question", true);

				--FlagsRaised_;
				CounterLabel_->setText(tr("Mines left: ") + QString::number(Mines_ - FlagsRaised_));
			}
		}
		else
		{
			Button->setText("|>");
			Button->setProperty("Flag", true);

			++FlagsRaised_;
			CounterLabel_->setText(tr("Mines left: ") + QString::number(Mines_ - FlagsRaised_));
		}
	}
}

void MainWindow::handleMiddleClick()
{
	MultiButton *Button = qobject_cast< MultiButton * >(sender());

	if (Button)
	{
		int Row = Button->property("Row").toInt();
		int Column = Button->property("Column").toInt();

		if (!Button->property("Opened").toBool())
		{
			return;
		}

		int count = MineCount_[Row][Column];
		if (count == 0)
		{
			return;
		}

		int FlagCount = 0;
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				int ni = Row + x;
				int nj = Column + y;
				if (ni >= 0 && ni < Rows_ && nj >= 0 && nj < Columns_)
				{
					MultiButton *neighbor = MineField_[ni][nj];
					if (neighbor->property("Flag").toBool())
						FlagCount++;
				}
			}
		}

		if (FlagCount == count)
		{
			for (int x = -1; x <= 1; ++x)
			{
				for (int y = -1; y <= 1; ++y)
				{
					int ni = Row + x;
					int nj = Column + y;
					if (ni >= 0 && ni < Rows_ && nj >= 0 && nj < Columns_)
					{
						MultiButton *neighbor = MineField_[ni][nj];
						if (!neighbor->property("Opened").toBool() && !neighbor->property("Flag").toBool())
						{
							if (HasMine_[ni][nj])
							{
								loseGame(neighbor);
								return;
							}
							else
							{
								openCell(ni, nj);
							}
						}
					}
				}
			}
		}
		else
		{
			for (int x = -1; x <= 1; ++x)
			{
				for (int y = -1; y <= 1; ++y)
				{
					int ni = Row + x;
					int nj = Column + y;
					if (ni >= 0 && ni < Rows_ && nj >= 0 && nj < Columns_)
					{
						MultiButton *neighbor = MineField_[ni][nj];
						if (!neighbor->property("Opened").toBool() && !neighbor->property("Flag").toBool() && !MinesShown_)
						{
							neighbor->setStyleSheet("background-color: lightgray; border: 1px solid black;");
						}
					}
				}
			}

			QTimer::singleShot(
				500,
				[=]()
				{
					for (int x = -1; x <= 1; ++x)
					{
						for (int y = -1; y <= 1; ++y)
						{
							int ni = Row + x;
							int nj = Column + y;
							if (ni >= 0 && ni < Rows_ && nj >= 0 && nj < Columns_)
							{
								MultiButton *neighbor = MineField_[ni][nj];
								if (!neighbor->property("Opened").toBool() && !neighbor->property("Flag").toBool() && !MinesShown_)
								{
									neighbor->setStyleSheet("background-color: darkcyan; border: 1px solid black;");
								}
							}
						}
					}
				});
		}
	}
}

void MainWindow::newGame()
{
	FlagsRaised_ = 0;
	createField();
}

void MainWindow::customGame()
{
	CustomGameDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		Rows_ = dialog.getRows();
		Columns_ = dialog.getColumns();
		Mines_ = dialog.getMines();
		createField();
	}
}

void MainWindow::exitGame()
{
	QApplication::quit();
}

void MainWindow::peek()
{
	toggleMines();
}

void MainWindow::invertClick()
{
	Inversion_ = !Inversion_;
}

void MainWindow::setEng()
{
	setLanguage("en_EN");
}

void MainWindow::setEs()
{
	setLanguage("es_ES");
}

void MainWindow::setRu()
{
	setLanguage("ru_RU");
}

void MainWindow::resizeEvent(QResizeEvent *Event)
{
	QSize NewSize = Event->size();

	int newSide = qMax(NewSize.width(), NewSize.height());
	resize(newSide, newSide);

	QWidget::resizeEvent(Event);
}

void MainWindow::closeEvent(QCloseEvent *Event)
{
	saveGameState(QCoreApplication::applicationDirPath() + "/game_save.ini");
	Event->accept();
}

void MainWindow::setLanguage(const QString &Locale)
{
	if (Translator_->load(":/i18n/minesweeper_" + Locale))
	{
		qApp->installTranslator(Translator_);
	}

	QSettings settings(QCoreApplication::applicationDirPath() + "/game_save.ini", QSettings::IniFormat);
	settings.setValue("Language", Locale);

	setWindowTitle(tr("Minesweeper"));

	QToolBar *oldToolBar = findChild< QToolBar * >();
	if (oldToolBar)
	{
		removeToolBar(oldToolBar);
		delete oldToolBar;
	}

	createToolBar();
}

void MainWindow::saveGameState(const QString &FileName)
{
	QSettings settings(FileName, QSettings::IniFormat);

	settings.beginGroup("Game");
	settings.setValue("Rows", Rows_);
	settings.setValue("Columns", Columns_);
	settings.setValue("Mines", Mines_);
	settings.setValue("OpenedCells", OpenedCells_);
	settings.setValue("ClosedCells", ClosedCells_);
	settings.setValue("FlagsRaised", FlagsRaised_);
	settings.setValue("Inversion", Inversion_);
	settings.setValue("FirstClick", FirstClick_);
	settings.endGroup();

	settings.beginGroup("Field");
	for (int i = 0; i < Rows_; ++i)
	{
		for (int j = 0; j < Columns_; ++j)
		{
			QString key = QString("%1_%2").arg(i).arg(j);
			settings.setValue(key + "_hasMine", HasMine_[i][j]);
			settings.setValue(key + "_mineCount", MineCount_[i][j]);
			settings.setValue(key + "_opened", MineField_[i][j]->property("Opened").toBool());
			settings.setValue(key + "_flagged", MineField_[i][j]->property("Flag").toBool());
			settings.setValue(key + "_questioned", MineField_[i][j]->property("Question").toBool());
		}
	}

	settings.endGroup();
}

void MainWindow::loadGameState(const QString &FileName)
{
	QSettings settings(FileName, QSettings::IniFormat);

	QString Language = settings.value("Language", "en_EN").toString();
	setLanguage(Language);

	settings.beginGroup("Game");
	Rows_ = settings.value("Rows").toInt();
	Columns_ = settings.value("Columns").toInt();
	Mines_ = settings.value("Mines").toInt();
	FlagsRaised_ = settings.value("FlagsRaised").toInt();
	createField();
	OpenedCells_ = settings.value("OpenedCells").toInt();
	ClosedCells_ = settings.value("ClosedCells").toInt();
	Inversion_ = settings.value("Inversion").toBool();
	FirstClick_ = settings.value("FirstClick").toBool();
	settings.endGroup();

	settings.beginGroup("Field");

	for (int i = 0; i < Rows_; ++i)
	{
		for (int j = 0; j < Columns_; ++j)
		{
			QString key = QString("%1_%2").arg(i).arg(j);
			HasMine_[i][j] = settings.value(key + "_hasMine").toBool();
			MineCount_[i][j] = settings.value(key + "_mineCount").toInt();
			MultiButton *Button = MineField_[i][j];
			Button->setProperty("Opened", settings.value(key + "_opened").toBool());
			Button->setProperty("Flag", settings.value(key + "_flagged").toBool());
			Button->setProperty("Question", settings.value(key + "_questioned").toBool());

			if (Button->property("Opened").toBool())
			{
				int count = MineCount_[i][j];
				Button->setText(count > 0 ? QString::number(count) : "");
				Button->setStyleSheet("color: darkcyan; background-color: lightgray; border: 1px solid black;");
			}
			else if (Button->property("Flag").toBool())
			{
				if (Button->property("Question").toBool())
				{
					Button->setText("?");
				}
				else
				{
					Button->setText("|>");
				}
			}
		}
	}

	settings.endGroup();
}
