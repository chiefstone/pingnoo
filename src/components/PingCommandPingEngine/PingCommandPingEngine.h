/*
 * Copyright (C) 2020 Adrian Carpenter
 *
 * This file is part of Pingnoo (https://github.com/nedrysoft/pingnoo)
 *
 * An open-source cross-platform traceroute analyser.
 *
 * Created by Adrian Carpenter on 22/01/2021.
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

#ifndef PINGNOO_COMPONENTS_PINGCOMMANDPINGENGINE_PINGCOMMANDPINGENGINE_H
#define PINGNOO_COMPONENTS_PINGCOMMANDPINGENGINE_PINGCOMMANDPINGENGINE_H

#include <IInterface>
#include <IPingEngine>
#include <IPingEngineFactory>

namespace Nedrysoft { namespace PingCommandPingEngine {
    class PingCommandPingTarget;

    /**
     * @brief       THe PingCommandPingEngine provides a command based ping engine implementation.
     */
    class PingCommandPingEngine :
            public Nedrysoft::RouteAnalyser::IPingEngine {

        private:
            Q_OBJECT

            Q_INTERFACES(Nedrysoft::RouteAnalyser::IPingEngine)

        public:
            /**
             * @brief       Constructs an PingCommandPingEngine for the given IP version.
             */
            explicit PingCommandPingEngine(Nedrysoft::Core::IPVersion version);

            /**
             * @brief       Destroys the PingCommandPingEngine.
             */
            ~PingCommandPingEngine();

            /**
             * @brief       Sets the measurement interval for this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::setInterval
             *
             * @param[in]   interval the interval between pings in milliseconds.
             *
             * @returns     returns true on success; otherwise false.
             */
            auto setInterval(int interval) -> bool override;

            /**
             * @brief       Returns the measurement interval.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::interval
             *
             * @returns     returns the measurement interval.
             */
            auto interval() -> int override;

            /**
             * @brief       Sets the reply timeout for this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::setTimeout
             *
             * @param[in]   timeout the time in milliseconds to wait for a reply.
             *
             * @returns     true on success; otherwise false.
             */
            auto setTimeout(int timeout) -> bool override;

            /**
             * @brief       Starts ping operations for this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::start
             *
             * @returns     true on success; otherwise false.
             */
            auto start() -> bool override;

            /**
             * @brief       Stops ping operations for this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::stop
             *
             * @returns     true on success; otherwise false.
             */
            auto stop() -> bool override;

            /**
             * @brief       Adds a ping target to this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::addTarget
             *
             * @param[in]   hostAddress the host address of the ping target.
             *
             * @returns     returns a pointer to the created ping target.
             */
            auto addTarget(QHostAddress hostAddress) -> Nedrysoft::RouteAnalyser::IPingTarget * override;

            /**
             * @brief       Adds a ping target to this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::addTarget
             *
             * @param[in]   hostAddress the host address of the ping target.
             * @param[in]   ttl the time to live to use.
             *
             * @returns     returns a pointer to the created ping target.
             */
            auto addTarget(QHostAddress hostAddress, int ttl) -> Nedrysoft::RouteAnalyser::IPingTarget * override;

            /**
             * @brief       Removes a ping target from this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::addTarget
             *
             * @param[in]   target the ping target to remove.
             *
             * @returns     true on success; otherwise false.
             */
            auto removeTarget(Nedrysoft::RouteAnalyser::IPingTarget *target) -> bool override;

            /**
             * @brief       Gets the epoch for this engine instance.
             *
             * @see         Nedrysoft::RouteAnalyser::IPingEngine::epoch
             *
             * @returns     the time epoch.
             */
            auto epoch() -> QDateTime;

            /**
             * @brief       Returns the list of ping targets for the engine.
             *
             * @returns     a QList containing the list of targets.
             */
            auto targets() -> QList<Nedrysoft::RouteAnalyser::IPingTarget *> override;

            /**
             * @brief       Transmits a single ping.
             *
             * @note        This is a blocking function.
             *
             * @param[in]   hostAddress the target host address.
             * @param[in]   ttl time to live for this packet.
             * @param[in]   timeout time in seconds to wait for response.
             *
             * @returns     the result of the ping.
             */
            auto singleShot(
                QHostAddress hostAddress,
                int ttl,
                double timeout
            ) -> Nedrysoft::RouteAnalyser::PingResult override;

        public:
            /**
             * @brief       Saves the configuration to a JSON object.
             *
             * @see         Nedrysoft::Core::IConfiguration::saveConfiguration
             *
             * @returns     the JSON configuration.
             */
            auto saveConfiguration() -> QJsonObject override;

            /**
             * @brief       Loads the configuration.
             *
             * @see         Nedrysoft::Core::IConfiguration::loadConfiguration
             *
             * @param[in]   configuration the configuration as JSON object.
             *
             * @returns     true if loaded; otherwise false.
             */
            auto loadConfiguration(QJsonObject configuration) -> bool override;

        private:
            auto emitResult(Nedrysoft::RouteAnalyser::PingResult pingResult) -> void;

            friend class PingCommandPingTarget;

        private:
            //! @cond

            QList<PingCommandPingTarget *> m_pingTargets;

            int m_interval;

            //! @endcond
    };
}}

#endif // PINGNOO_COMPONENTS_PINGCOMMANDPINGENGINE_PINGCOMMANDPINGENGINE_H
