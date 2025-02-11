/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of Pingnoo (https://github.com/nedrysoft/pingnoo)
 *
 * An open-source cross-platform traceroute analyser.
 *
 * Created by Adrian Carpenter on 27/03/2020.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGlobal>

#include "RouteAnalyserComponent.h"

#include "ColourDialog.h"
#include "IRouteEngine.h"
#include "LatencyRibbonGroup.h"
#include "LatencySettings.h"
#include "LatencySettingsPage.h"
#include "NewTargetDialog.h"
#include "NewTargetRibbonGroup.h"
#include "PingResult.h"
#include "RouteAnalyser.h"
#include "RouteAnalyserConstants.h"
#include "RouteAnalyserMenuItem.h"
#include "TargetManager.h"
#include "TargetSettings.h"
#include "TargetSettingsPage.h"
#include "ViewportRibbonGroup.h"

#include <CoreConstants>
#include <ICommand>
#include <ICommandManager>
#include <IContextManager>
#include <IEditorManager>
#include <IRibbonBarManager>
#include <IRibbonPage>
#include <ISystemTrayIcon>
#include <ISystemTrayIconManager>
#if defined(Q_OS_MACOS)
#include <MacHelper>
#include <MacMenubarIcon>
#include <MacPopover>
#include <QPointer>
#endif
#include <QDir>
#include <QDirIterator>
#if !defined(Q_OS_MACOS)
#include <QGuiApplication>
#include <QScreen>
#endif
#include <RibbonAction>
#include <RibbonDropButton>
#include <QVBoxLayout>
#include <PopoverWindow.h>

#if defined(Q_OS_WINDOWS)
#include <windows.h>
#include <Shellapi.h>
#endif

constexpr auto FontBasePath = ":/Nedrysoft/RouteAnalyser/Roboto_Mono/static";

RouteAnalyserComponent::RouteAnalyserComponent() :
        m_newTargetGroupWidget(nullptr),
        m_latencyGroupWidget(nullptr),
        m_viewportGroupWidget(nullptr),
        m_latencySettingsPage(nullptr),
        m_targetSettingsPage(nullptr),
        m_newTargetAction(nullptr),
        m_latencySettings(nullptr) {

}

RouteAnalyserComponent::~RouteAnalyserComponent() {

}

auto RouteAnalyserComponent::initialiseEvent() -> void {
    qRegisterMetaType<Nedrysoft::RouteAnalyser::PingResult>("Nedrysoft::RouteAnalyser::PingResult");
    qRegisterMetaType<Nedrysoft::RouteAnalyser::RouteList>("Nedrysoft::RouteAnalyser::RouteList");
    qRegisterMetaType<Nedrysoft::RouteAnalyser::IPingEngineFactory *>("Nedrysoft::RouteAnalyser::IPingEngineFactory *");
}

auto RouteAnalyserComponent::finaliseEvent() -> void {
    auto editorList = Nedrysoft::ComponentSystem::getObjects<Nedrysoft::RouteAnalyser::RouteAnalyserEditor>();

    if (!editorList.isEmpty()) {
        qDeleteAll(editorList);
    }

    if (m_latencySettingsPage) {
        Nedrysoft::ComponentSystem::removeObject(m_latencySettingsPage);

        delete m_latencySettingsPage;
    }

    if (m_targetSettingsPage) {
        Nedrysoft::ComponentSystem::removeObject(m_targetSettingsPage);

        delete m_targetSettingsPage;
    }

    if (m_latencySettings) {
        Nedrysoft::ComponentSystem::removeObject(m_latencySettings);

        delete m_latencySettings;
    }

    if (m_targetSettings) {
        Nedrysoft::ComponentSystem::removeObject(m_targetSettings);

        delete m_targetSettings;
    }

    if (m_viewportGroupWidget) {
        Nedrysoft::ComponentSystem::removeObject(m_viewportGroupWidget);
    }

    if (m_latencyGroupWidget) {
        Nedrysoft::ComponentSystem::removeObject(m_latencyGroupWidget);
    }

    if (m_newTargetAction) {
        delete m_newTargetAction;
    }

    delete Nedrysoft::RouteAnalyser::TargetManager::getInstance();
}

auto RouteAnalyserComponent::initialisationFinishedEvent() -> void {
    auto contextManager = Nedrysoft::Core::IContextManager::getInstance();
#if defined(Q_OS_MACOS)
    Nedrysoft::MacHelper::MacHelper::disableAppNap(
            QT_TR_NOOP("App Nap has been disabled as it interferes with thread timing.")
    );
#endif
    if (contextManager) {
        m_editorContextId = contextManager->registerContext(Pingnoo::Constants::routeAnalyserContext);

        connect(contextManager, &Nedrysoft::Core::IContextManager::contextChanged,
                [&](int newContext, int previousContext) {
                    Q_UNUSED(newContext)
                    Q_UNUSED(previousContext)
                });
    }

    auto core = Nedrysoft::Core::ICore::getInstance();

    if (core) {
        connect(core, &Nedrysoft::Core::ICore::coreOpened, [&]() {
            auto commandManager = Nedrysoft::Core::ICommandManager::getInstance();

            if (commandManager) {
                // create New Target... action

                m_newTargetAction =  new QAction(tr("New Target..."));

                connect(m_newTargetAction, &QAction::triggered, [=]() {
                    Nedrysoft::RouteAnalyser::NewTargetDialog newTargetDialog;

                    if (newTargetDialog.exec()) {
                        auto editorManager = Nedrysoft::Core::IEditorManager::getInstance();

                        if (editorManager) {
                            Nedrysoft::RouteAnalyser::RouteAnalyserEditor *editor =
                                    new Nedrysoft::RouteAnalyser::RouteAnalyserEditor;

                            editor->setPingEngine(newTargetDialog.pingEngineFactory());
                            editor->setTarget(newTargetDialog.pingTarget());
                            editor->setIPVersion(newTargetDialog.ipVersion());
                            editor->setInterval(newTargetDialog.interval());

                            editorManager->openEditor(editor);
                        }
                    }
                });

                // register File/New Target... menu option global context

                auto command = commandManager->registerAction(
                    m_newTargetAction,
                    Nedrysoft::RouteAnalyser::Constants::Commands::NewTarget
                );

                auto menu = commandManager->findMenu(Nedrysoft::Core::Constants::Menus::File);

                menu->appendCommand(command, Nedrysoft::Core::Constants::MenuGroups::FileNew);

                auto ribbonBarManager = Nedrysoft::Core::IRibbonBarManager::getInstance();

                if (ribbonBarManager) {
                    auto clipboardCopyAction = new Nedrysoft::Ribbon::RibbonAction;

                    ribbonBarManager->registerAction(
                        clipboardCopyAction,
                        Nedrysoft::Core::Constants::RibbonCommands::ClipboardCopy,
                        m_editorContextId
                    );

                    connect(
                        clipboardCopyAction,
                        &Nedrysoft::Ribbon::RibbonAction::ribbonEvent,
                        [=](Nedrysoft::Ribbon::Event *generalEvent) {

                            if (generalEvent->type()==Nedrysoft::Ribbon::EventType::DropButtonClicked) {
                                auto event =
                                    reinterpret_cast<Nedrysoft::Ribbon::DropButtonClickedEvent *>(generalEvent);

                                if (event->dropDown()) {
                                    handleClipboardMenu(
                                        event->button()->mapToGlobal(event->button()->rect().bottomLeft())
                                    );
                                }
                            }
                        }
                    );
                }
            }
        });

        m_targetSettings = new Nedrysoft::RouteAnalyser::TargetSettings;

        Nedrysoft::ComponentSystem::addObject(m_targetSettings);

        m_targetSettings->loadFromFile();

        m_latencySettings = new Nedrysoft::RouteAnalyser::LatencySettings;

        Nedrysoft::ComponentSystem::addObject(m_latencySettings);

        m_latencySettings->loadFromFile();

        auto ribbonBarManager = Nedrysoft::Core::IRibbonBarManager::getInstance();

        if (ribbonBarManager) {
            auto ribbonPage = ribbonBarManager->addPage(
                    tr("Route Analyser"),
                    Pingnoo::Constants::ribbonRouteAnalyserPage,
                    0.1f );

            ribbonBarManager->selectPage(Pingnoo::Constants::ribbonRouteAnalyserPage);

            m_newTargetGroupWidget = new Nedrysoft::RouteAnalyser::NewTargetRibbonGroup;
            m_latencyGroupWidget = new Nedrysoft::RouteAnalyser::LatencyRibbonGroup;
            m_viewportGroupWidget = new Nedrysoft::RouteAnalyser::ViewportRibbonGroup;

            ribbonPage->addGroup(
                    tr("New Target"),
                    Pingnoo::Constants::ribbonRouteAnalyserNewTargetGroup,
                    m_newTargetGroupWidget );

            ribbonPage->addGroup(
                    tr("Latency"),
                    Pingnoo::Constants::ribbonRouteAnalyserLatencyGroup,
                    m_latencyGroupWidget );

            ribbonPage->addGroup(
                    tr("Viewport"),
                    Pingnoo::Constants::ribbonRouteAnalyserViewportGroup,
                    m_viewportGroupWidget );
        }

        m_latencySettingsPage = new Nedrysoft::RouteAnalyser::LatencySettingsPage;
        m_targetSettingsPage = new Nedrysoft::RouteAnalyser::TargetSettingsPage;

        Nedrysoft::ComponentSystem::addObject(m_latencySettingsPage);
        Nedrysoft::ComponentSystem::addObject(m_targetSettingsPage);
        Nedrysoft::ComponentSystem::addObject(m_newTargetGroupWidget);
        Nedrysoft::ComponentSystem::addObject(m_viewportGroupWidget);
        Nedrysoft::ComponentSystem::addObject(m_latencyGroupWidget);
    }

    auto dirIterator = QDirIterator(FontBasePath);

    while(dirIterator.hasNext()) {
        dirIterator.next();

        QFontDatabase::addApplicationFont(dirIterator.filePath());
    }

    auto systemTrayIconManager = Nedrysoft::Core::ISystemTrayIconManager::getInstance();

    auto systemTrayIcon = systemTrayIconManager->createIcon();

    systemTrayIcon->setColour(Qt::black);

    connect(Nedrysoft::Core::mainWindow(), &QObject::destroyed, [=](QObject *) {
        delete systemTrayIcon;
    });

    connect(
        systemTrayIcon,
        &Nedrysoft::Core::ISystemTrayIcon::clicked,
        [=](Nedrysoft::Core::ISystemTrayIcon::MouseButton button) {

            if (button==Nedrysoft::Core::ISystemTrayIcon::MouseButton::Left) {
#if defined(Q_OS_MACOS)
                auto popover = new Nedrysoft::MacHelper::MacPopover;

                QPointer<QWidget> popoverWidget = new QWidget;
#else
                auto popoverWidget = new Nedrysoft::RouteAnalyser::PopoverWindow(Nedrysoft::Core::mainWindow());

                auto iconRect = systemTrayIcon->geometry();
#endif
                auto contentLayout = new QVBoxLayout;

                for (int i = 0; i < 5; i++) {
                    contentLayout->addWidget(new Nedrysoft::RouteAnalyser::RouteAnalyserMenuItem);
                }

#if defined(Q_OS_MACOS)
                popoverWidget->setLayout(contentLayout);

                popover->show(
                    systemTrayIcon->menubarIcon(),
                    popoverWidget,
                    QSize(popoverWidget->minimumWidth(), popoverWidget->sizeHint().height()),
                    Nedrysoft::MacHelper::MacPopover::Edge::MaxYEdge
                );
#elif defined(Q_OS_WINDOWS)

                popoverWidget->setLayout(contentLayout);

                APPBARDATA appbarData;

                memset(&appbarData, 0, sizeof(appbarData));

                appbarData.cbSize = sizeof(appbarData);

                SHAppBarMessage(ABM_GETTASKBARPOS, &appbarData);

                for (auto screen : qGuiApp->screens()) {
                    if (screen->geometry().contains(iconRect)) {
                        QRect popoverRect = QRect(QPoint(0,0), popoverWidget->sizeHint());

                        switch(appbarData.uEdge) {
                            case ABE_TOP: {
                                popoverRect.moveTopLeft(
                                        QPoint(
                                                iconRect.center().x()-(popoverRect.width()/2),
                                                iconRect.bottom()
                                        )
                                );

                                if (popoverRect.right()>screen->geometry().right()) {
                                    popoverRect.moveRight(screen->geometry().right());
                                }

                                popoverWidget->move(popoverRect.topLeft());

                                break;
                            }

                            case ABE_BOTTOM: {
                                popoverRect.moveBottomRight(
                                        QPoint(
                                                iconRect.center().x()+(popoverRect.width()/2),
                                                iconRect.top()
                                        )
                                );

                                if (popoverRect.right()>screen->geometry().right()) {
                                    popoverRect.moveRight(screen->geometry().right());
                                }

                                popoverWidget->move(popoverRect.topLeft());

                                break;
                            }

                            case ABE_LEFT: {
                                popoverRect.moveTopLeft(
                                        QPoint(
                                                iconRect.right(),
                                                iconRect.center().y()-(popoverRect.height()/2)

                                        )
                                );

                                if (popoverRect.bottom()>screen->geometry().bottom()) {
                                    popoverRect.moveBottom(screen->geometry().bottom());
                                }

                                popoverWidget->move(popoverRect.topLeft());

                                break;
                            }

                            case ABE_RIGHT: {
                                popoverRect.moveTopLeft(
                                        QPoint(
                                                iconRect.right(),
                                                iconRect.center().y()-(popoverRect.height()/2)

                                        )
                                );

                                if (popoverRect.bottom()>screen->geometry().bottom()) {
                                    popoverRect.moveBottom(screen->geometry().bottom());
                                }

                                popoverWidget->move(popoverRect.topLeft());

                                break;
                            }
                        }

                        break;
                    }
                }

                popoverWidget->show();
#endif
                connect(this, &RouteAnalyserComponent::destroyed, [=]() {
                    if (popoverWidget) {
                        delete popoverWidget;
                    }
                });
            } else if (button==Nedrysoft::Core::ISystemTrayIcon::MouseButton::Right) {
#if defined(Q_OS_MACOS)
                auto contentMenu = Nedrysoft::Core::ICore::getInstance()->applicationContextMenu();

                auto contextObject = new QObject(this);

                connect(
                    systemTrayIcon,
                    &Nedrysoft::Core::ISystemTrayIcon::menuClosed,
                    contextObject,
                    [contentMenu, contextObject](QMenu *menu) {

                        contentMenu->deleteLater();
                        contextObject->deleteLater();
                    }
                );

                systemTrayIcon->showMenu(contentMenu->menu());
#endif
            }
        }
    );
}

auto RouteAnalyserComponent::contextId() -> int {
    return m_editorContextId;
}

auto RouteAnalyserComponent::handleClipboardMenu(QPoint position) -> void {
    QMenu menu;

    auto editorManager = Nedrysoft::Core::IEditorManager::getInstance();

    auto routeAnalyserEditor =
        qobject_cast<Nedrysoft::RouteAnalyser::RouteAnalyserEditor *>(editorManager->currentEditor());

    if (routeAnalyserEditor==nullptr) {
        return;
    }

    auto copyTableAsText = menu.addAction(tr("Copy Table as Text"));
    auto copyTableAsPDF = menu.addAction(tr("Copy Table as PDF"));
    auto copyTableAsImage = menu.addAction(tr("Copy Table as Image"));
    auto copyTableAsCSV = menu.addAction(tr("Copy Table as CSV"));
    auto copyGraphsAsImage = menu.addAction(tr("Copy Graphs as Image"));
    auto copyGraphsAsPDF = menu.addAction(tr("Copy Graphs as PDF"));
    auto CopyTableAndGraphsAsImage = menu.addAction(tr("Copy Table and Graphs as Image"));
    auto CopyTableAndGraphsAsPDF = menu.addAction(tr("Copy Table and Graphs as PDF"));

    menu.addAction(copyTableAsText);
    menu.addAction(copyTableAsPDF);
    menu.addAction(copyTableAsImage);
    menu.addAction(copyTableAsCSV);
    menu.addAction(copyGraphsAsImage);
    menu.addAction(copyGraphsAsPDF);
    menu.addAction(CopyTableAndGraphsAsImage);
    menu.addAction(CopyTableAndGraphsAsPDF);

    auto selectedAction = menu.exec(position);

    if (selectedAction==copyTableAsText) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::TableAsText,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    } else if (selectedAction==copyTableAsPDF) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::TableAsPDF,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    } else if (selectedAction==copyTableAsImage) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::TableAsImage,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    } else if (selectedAction==copyTableAsCSV) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::TableAsCSV,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    } else if (selectedAction==copyGraphsAsImage) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::GraphsAsImage,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    } else if (selectedAction==copyGraphsAsPDF) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::GraphsAsPDF,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    } else if (selectedAction==CopyTableAndGraphsAsImage) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::TableAndGraphsAsImage,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    } else if (selectedAction==CopyTableAndGraphsAsPDF) {
        routeAnalyserEditor->generateOutput(
            Nedrysoft::RouteAnalyser::OutputType::TableAndGraphsAsPDF,
            Nedrysoft::RouteAnalyser::OutputTarget::Clipboard
        );
    }
}