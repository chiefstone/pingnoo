/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of Pingnoo (https://github.com/nedrysoft/pingnoo)
 *
 * An open-source cross-platform traceroute analyser.
 *
 * Created by Adrian Carpenter on 27/06/2021.
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

#ifndef PINGNOO_COMPONENTS_CORE_RIBBONACTIONPROXY_H
#define PINGNOO_COMPONENTS_CORE_RIBBONACTIONPROXY_H

#include <QAction>
#include <QPointer>
#include <RibbonAction>
using namespace Nedrysoft::Ribbon;
namespace Nedrysoft { namespace Core {
    /**
     * @brief       The RibbonActionProxy class is used to proxy actions depending on context.
     *
     * @details     A proxy class for a RibbonAction, allows the action to be proxied through, the active RibbonAction
     *              can be switched depending on the current context of the application.
     */
    class RibbonActionProxy :
            public Nedrysoft::Ribbon::RibbonAction {

        private:
            Q_OBJECT

        public:

            /**
             * @brief       Constructs a new RibbonDropButtonActionProxy instance which is a child of the parent.
             *
             * @param[in]   parent the owner object.
             */
            explicit RibbonActionProxy(QObject *parent = nullptr);

            /**
             * @brief       Sets the currently active RibbonDropButtonAction.
             *
             * @param[in]   action the action to be proxied.
             */
            auto setActive(RibbonAction *action) -> void;

            /**
             * @brief       Retuns the currently active action.
             *
             * @returns     the current active action being proxied.
             */
            auto activeAction() -> Nedrysoft::Ribbon::RibbonAction *;


            auto triggerEvent(Nedrysoft::Ribbon::Event* event) -> void override
            {
                throw std::logic_error("The method or operation is not implemented.");
            }

    protected:
            /**
             * @brief       Connects the current action to the proxy.
             */
            auto connectAction() -> void;

            /**
             * @brief       Disconnects the current action from the proxy.
             */
            auto disconnectAction() -> void;

        private:
            //! @cond

            QPointer<Nedrysoft::Ribbon::RibbonAction> m_action;

            //! @endcond
    };
}}

#endif // PINGNOO_COMPONENTS_CORE_RIBBONACTIONPROXY_H
