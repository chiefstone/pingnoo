/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of Pingnoo (https://github.com/nedrysoft/pingnoo)
 *
 * An open-source cross-platform traceroute analyser.
 *
 * Created by Adrian Carpenter on 19/06/2021.
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
#ifndef PINGNOO_COMPONENTS_CORE_IHOSTMASKERMANAGER_H
#define PINGNOO_COMPONENTS_CORE_IHOSTMASKERMANAGER_H

#include "CoreSpec.h"
#include "IComponentManager.h"

#include <IInterface>
#include <QObject>

namespace Nedrysoft {
	namespace Core {

		class IHostMasker;

		namespace HostMask {

			Q_NAMESPACE
				enum  HostMaskType {
				Screen,
				Output,
				Clipboard
			};
			Q_ENUM_NS(HostMaskType)


		}

		class NEDRYSOFT_CORE_DLLSPEC IHostMaskerManager :
			public Nedrysoft::ComponentSystem::IInterface {

		private:
			Q_OBJECT
				Q_INTERFACES(Nedrysoft::ComponentSystem::IInterface)

		public:
			static auto getInstance() -> IHostMaskerManager* {
				return ComponentSystem::getObject<IHostMaskerManager>();
			}

			virtual auto enabled(Nedrysoft::Core::HostMask::HostMaskType type) -> bool = 0;
			virtual auto setEnabled(Nedrysoft::Core::HostMask::HostMaskType type, bool enabled) -> void = 0;
			virtual auto add(Nedrysoft::Core::IHostMasker* hostMasker) -> void = 0;
			virtual auto remove(Nedrysoft::Core::IHostMasker* hostMasker) -> void = 0;
			virtual auto maskers() -> QList<Nedrysoft::Core::IHostMasker*> = 0;

			virtual ~IHostMaskerManager() = default;

		public:
			Q_SIGNAL void maskStateChanged(Nedrysoft::Core::HostMask::HostMaskType type, bool state);
		};

	}
}

Q_DECLARE_INTERFACE(Nedrysoft::Core::IHostMaskerManager, "com.nedrysoft.core.IHostMaskerManager/1.0.0")

#endif // PINGNOO_COMPONENTS_CORE_IHOSTMASKERMANAGER_H