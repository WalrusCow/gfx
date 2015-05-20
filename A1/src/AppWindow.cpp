#include <QtWidgets>
#include <QGLFormat>
#include <iostream>
#include "AppWindow.hpp"

const int AppWindow::SLOW_MS = 800;
const int AppWindow::MEDIUM_MS = 500;
const int AppWindow::FAST_MS = 200;

void AppWindow::gameTick() {
  game.tick();
}

AppWindow::AppWindow() : game(10, 20) {
  setWindowTitle("488 Tetrominoes on the Wall");

  gameTicker = new QTimer(this);
  connect(gameTicker, SIGNAL(timeout()), this, SLOT(gameTick()));
  gameTicker->start(SLOW_MS);

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

  // Other handlers
  switch (key) {
    case Qt::Key_Space:
      game.drop();
      break;
    case Qt::Key_Left:
      game.moveLeft();
      break;
    case Qt::Key_Right:
      game.moveRight();
      break;
    case Qt::Key_Up:
      game.rotateCCW();
      break;
    case Qt::Key_Down:
      game.rotateCW();
      break;

    case Qt::Key_Escape:
      QCoreApplication::instance()->quit();
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

void AppWindow::newGame() {
  game.reset();
}

void AppWindow::slowSpeed() {
  gameTicker->setInterval(SLOW_MS);
}
void AppWindow::medSpeed() {
  gameTicker->setInterval(MEDIUM_MS);
}
void AppWindow::fastSpeed() {
  gameTicker->setInterval(FAST_MS);
}

void AppWindow::multiMode() {
  m_viewer->setMode(Viewer::DrawMode::MULTICOLOUR);
}
void AppWindow::wireMode() {
  m_viewer->setMode(Viewer::DrawMode::WIRE);
}
void AppWindow::faceMode() {
  m_viewer->setMode(Viewer::DrawMode::FACE);
}
void AppWindow::resetView() {
  m_viewer->resetView();
}


void AppWindow::createActions() {
  QAction* a;
  // Application menu
  a = newMenuAction("&New Game", nullptr,
      "Start a new game", appMenuActions, Qt::Key_N);
  connect(a, SIGNAL(triggered()), this, SLOT(newGame()));

  a = newMenuAction("&Reset", nullptr,
      "Reset the view", appMenuActions, Qt::Key_R);
  connect(a, SIGNAL(triggered()), this, SLOT(resetView()));

  a = newMenuAction("&Quit", nullptr,
      "Exit the program", appMenuActions, Qt::Key_Q);
  connect(a, SIGNAL(triggered()), this, SLOT(close()));

  // Draw menu
  QActionGroup* drawGroup = new QActionGroup(this);
  a = newMenuAction("&Face", drawGroup,
      "Fill faces", drawMenuActions, Qt::Key_F);
  connect(a, SIGNAL(triggered()), this, SLOT(faceMode()));

  a = newMenuAction("&Wire-frame", drawGroup,
      "Draw wire frames", drawMenuActions, Qt::Key_W);
  connect(a, SIGNAL(triggered()), this, SLOT(wireMode()));

  a = newMenuAction("&Multicoloured", drawGroup,
      "Each cube has 6 unique colours", drawMenuActions, Qt::Key_M);
  connect(a, SIGNAL(triggered()), this, SLOT(multiMode()));

  drawGroup->actions().first()->setChecked(true);

  // Speed menu
  QActionGroup* speedGroup = new QActionGroup(this);
  a = newMenuAction("Slow", speedGroup,
      "Slow speed", speedMenuActions, Qt::Key_1);
  connect(a, SIGNAL(triggered()), this, SLOT(slowSpeed()));

  a = newMenuAction("Medium", speedGroup,
      "Medium speed", speedMenuActions, Qt::Key_2);
  connect(a, SIGNAL(triggered()), this, SLOT(medSpeed()));

  a = newMenuAction("Fast", speedGroup,
      "Fast speed", speedMenuActions, Qt::Key_3);
  connect(a, SIGNAL(triggered()), this, SLOT(fastSpeed()));

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
    int shortcut) {

  QAction* action = new QAction(tr(title.c_str()), this);
  action->setStatusTip(tr(tip.c_str()));
  if (actionGroup) {
    action->setCheckable(true);
    action->setActionGroup(actionGroup);
  }
  shortcutActions.insert({shortcut, action});
  menuList.push_back(action);
  return action;
}
