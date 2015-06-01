#include <QtWidgets>
#include <iostream>
#include "AppWindow.hpp"

AppWindow::AppWindow() {
  setWindowTitle("488 Assignment Two");

  QGLFormat glFormat;
  glFormat.setVersion(3,3);
  glFormat.setProfile(QGLFormat::CoreProfile);
  glFormat.setSampleBuffers(true);

  QVBoxLayout *layout = new QVBoxLayout;
  viewer = new Viewer(glFormat, this);
  layout->addWidget(viewer);
  setCentralWidget(new QWidget);
  centralWidget()->setLayout(layout);

  createActions();
  createMenu();
}

void AppWindow::createActions() {
  newMenuAction("&Quit", nullptr, "Exit the program",
      quitMenuActions, Qt::Key_Q, [this] () {
    close();
  })->setShortcuts(QKeySequence::Quit);

  newMenuAction("&Reset", nullptr, "Reset all transformations",
      quitMenuActions, Qt::Key_A, [this] () {
    viewer->resetView();
  });

  QActionGroup* modeGroup = new QActionGroup(this);
  // Model actions
  newMenuAction("&Rotate Model", modeGroup, "Rotate the model",
      modeMenuActions, Qt::Key_R, [this] () {
      viewer->setMode(Viewer::Mode::MODEL_ROTATE);
  });
  newMenuAction("&Translate Model", modeGroup, "Translate the model",
      modeMenuActions, Qt::Key_T, [this] () {
    viewer->setMode(Viewer::Mode::MODEL_TRANSLATE);
  });
  newMenuAction("&Scale Model", modeGroup, "Scale the model",
      modeMenuActions, Qt::Key_S, [this] () {
    viewer->setMode(Viewer::Mode::MODEL_SCALE);
  });

  // View actions
  newMenuAction("Rotate View", modeGroup, "Rotate the view",
      modeMenuActions, Qt::Key_O, [this] () {
    viewer->setMode(Viewer::Mode::VIEW_ROTATE);
  });
  newMenuAction("Translate View", modeGroup, "Translate the view",
      modeMenuActions, Qt::Key_N, [this] () {
    viewer->setMode(Viewer::Mode::VIEW_TRANSLATE);
  });
  newMenuAction("View Perspective", modeGroup, "Change view perspective",
      modeMenuActions, Qt::Key_P, [this] () {
    viewer->setMode(Viewer::Mode::VIEW_PERSPECTIVE);
  });

  newMenuAction("Viewport", modeGroup, "Change the viewport",
      modeMenuActions, Qt::Key_V, [this] () {
    viewer->setMode(Viewer::Mode::VIEWPORT);
  });

  // Display the first option as checked
  modeGroup->actions().first()->setChecked(true);
}

void AppWindow::createMenu() {
  appMenu = menuBar()->addMenu(tr("&Application"));
  for (auto& action : quitMenuActions) {
    appMenu->addAction(action);
  }

  modeMenu = menuBar()->addMenu(tr("&Mode"));
  for (auto& action : modeMenuActions) {
    modeMenu->addAction(action);
  }
}

void AppWindow::keyPressEvent(QKeyEvent* event) {
  int key = event->key();
  // Trigger menu actions
  auto it = shortcutActions.find(key);
  if (it != shortcutActions.end()) {
    it->second->trigger();
    return;
  }
  // Other handlers?
}


QAction* AppWindow::newMenuAction(
    const std::string& title,
    QActionGroup* actionGroup,
    const std::string& tip,
    std::list<QAction*>& menuList,
    int shortcut,
    const std::function<void()> onTrigger) {

  QAction* action = new QAction(tr(title.c_str()), this);
  action->setStatusTip(tr(tip.c_str()));
  if (actionGroup) {
    action->setCheckable(true);
    action->setActionGroup(actionGroup);
  }

  connect(action, &QAction::triggered, this, onTrigger);
  shortcutActions.insert({shortcut, action});
  menuList.push_back(action);

  return action;
}

