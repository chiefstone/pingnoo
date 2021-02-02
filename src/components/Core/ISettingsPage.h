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

#ifndef NEDRYSOFT_CORE_ISETTINGSPAGE_H
#define NEDRYSOFT_CORE_ISETTINGSPAGE_H

#include "ComponentSystem/IInterface.h"
#include "CoreSpec.h"

#include <QWidget>

namespace Nedrysoft::Core {
    /**
     * @brief       The ISettingsPage interface defines a settings page.
     */
    class NEDRYSOFT_CORE_DLLSPEC ISettingsPage :
            public Nedrysoft::ComponentSystem::IInterface {

        private:
            Q_OBJECT

            Q_INTERFACES(Nedrysoft::ComponentSystem::IInterface)

        public:
            /**
             * @brief       Returns the widget for the settings page.
             *
             * @returns     the widget
             */
            virtual auto widget() -> QWidget = 0;

            /**
             * @brief       Returns the display name for the settings page.
             *
             * @returns     the displayed name of the settings page.
             */
            virtual auto displayName() -> QString = 0;
    };
}

Q_DECLARE_INTERFACE(Nedrysoft::Core::ISettingsPage, "com.nedrysoft.core.ISettingsPage/1.0.0")

#endif // NEDRYSOFT_CORE_ISETTINGSPAGE_H
