#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() : game(10, 20) {
  setWindowTitle("488 Tetrominoes on the Wall");

  gameTicker = new QTimer(this);
  connect(gameTicker, &QTimer::timeout, this, [this] {
    game.tick();
  });
  gameTicker->start(500);

  QGLFormat glFormat;
  glFormat.setVersion(3, 3);
  glFormat.setProfile(QGLFormat::CoreProfile);
  glFormat.setSampleBuffers(true);

  QVBoxLayout *layout = new QVBoxLayout;
  m_viewer = new Viewer(glFormat, this);
  layout->addWidget(m_viewer);
  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);

  m_viewer->viewGame(&game);
  m_viewer->show();

  createActions();
  createMenu();
}

void AppWindow::keyPressEvent(QKeyEvent *event) {
  int key = event->key();
  auto it = shortcutActions.find(key);
  if (it != shortcutActions.end()) {
    // We have a matching menu shortcut
    it->second->trigger();
    return;
  }

  switch (key) {
    case Qt::Key_Escape:
      QCoreApplication::instance()->quit();
      break;
    case Qt::Key_T:
      std::cerr << "Hello!" << std::endl;
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

void AppWindow::newGame() {
  game.reset();
}

void AppWindow::createActions() {
  // Application menu
  newMenuAction("&New Game", nullptr,
      "Start a new game", appMenuActions, Qt::Key_N, [this] {
    newGame();
  });

  newMenuAction("&Reset", nullptr,
      "Reset the view", appMenuActions, Qt::Key_R, [this] {
    m_viewer->resetView();
  });

  newMenuAction("&Quit", nullptr,
      "Exit the program", appMenuActions, Qt::Key_Q, [this] {
    QMainWindow::close();
  })->setShortcuts(QKeySequence::Quit);

  QActionGroup* drawGroup = new QActionGroup(this);

  // Draw menu
  newMenuAction("&Face", drawGroup,
      "Fill faces", drawMenuActions, Qt::Key_F, [this] {
    m_viewer->setMode(Viewer::DrawMode::FACE);
  });

  newMenuAction("&Wire-frame", drawGroup,
      "Draw wire frames", drawMenuActions, Qt::Key_W, [this] {
    m_viewer->setMode(Viewer::DrawMode::WIRE);
  });

  newMenuAction("&Multicoloured", drawGroup,
      "Each cube has 6 unique colours", drawMenuActions, Qt::Key_M, [this] {
    m_viewer->setMode(Viewer::DrawMode::FACE);
  });

  drawGroup->actions().first()->setChecked(true);

  QActionGroup* speedGroup = new QActionGroup(this);

  // Speed menu
  newMenuAction("Slow", speedGroup,
      "Slow speed", speedMenuActions, Qt::Key_1, [this] {
    //m_viewer->setMode(Viewer::DrawMode::FACE);
  });

  newMenuAction("Medium", speedGroup,
      "Medium speed", speedMenuActions, Qt::Key_2, [this] {
    //m_viewer->setMode(Viewer::DrawMode::FACE);
  });

  newMenuAction("Fast", speedGroup,
      "Fast speed", speedMenuActions, Qt::Key_3, [this] {
    //m_viewer->setMode(Viewer::DrawMode::FACE);
  });

  speedGroup->actions().first()->setChecked(true);
}

void AppWindow::createMenu() {
  appMenu = menuBar()->addMenu(tr("&Application"));
  for (auto& kv : appMenuActions) {
    appMenu->addAction(kv);
  }

  drawMenu = menuBar()->addMenu(tr("&Draw Mode"));
  for (auto& kv : drawMenuActions) {
    drawMenu->addAction(kv);
  }

  speedMenu = menuBar()->addMenu(tr("&Speed"));
  for (auto& kv : speedMenuActions) {
    speedMenu->addAction(kv);
  }
}

QAction* AppWindow::newMenuAction(
    const std::string& title,
    QActionGroup* actionGroup,
    const std::string& tip,
    std::list<QAction*>& menuList,
    int shortcut,
    const std::function<void()>& onTrigger) {

  QAction* action = new QAction(tr(title.c_str()), this);
  action->setStatusTip(tr(tip.c_str()));
  if (actionGroup) {
    action->setCheckable(true);
    action->setActionGroup(actionGroup);
  }
  shortcutActions.insert({shortcut, action});
  menuList.push_back(action);
  connect(action, &QAction::triggered, this, onTrigger);
  return action;
}
