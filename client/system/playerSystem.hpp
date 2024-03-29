/*
** EPITECH PROJECT, 2023
** R-type
** File description:
** inputSystem
*/

#ifndef PLAYERSYSTEM_HPP_
    #define PLAYERSYSTEM_HPP_

    #include "../../ecs/system/ISystem.hpp"
    #include "../../ecs/registry/registry.hpp"
    #include "../../ecs/event/shoot.hpp"
    #include "../../ecs/event/ennemy.hpp"

class PlayerSystem : public ISystem {
    public:
        PlayerSystem() = delete;
        PlayerSystem(registry &reg): _reg(reg) {};
        ~PlayerSystem() = default;
        void operator()() override {
            for (auto &tmp : _reg._eventManager.getEvent<keyPressed>()) {
                auto &velocity = _reg.getComponent<Velocity>();
                auto &position = _reg.getComponent<Position>();
                auto &health = _reg.getComponent<Health>();
                if (!position[_reg._player] && !velocity[_reg._player])
                    return;
                if (!health[_reg._player] || health[_reg._player].value().health <= 0)
                    return;
                switch(tmp->_key) {
                    case sf::Keyboard::Right:
                        velocity[_reg._player].value().x_speed = 2;
                        velocity[_reg._player].value().y_speed = 0;
                        break;
                    case sf::Keyboard::Left:
                        velocity[_reg._player].value().x_speed = -2;
                        velocity[_reg._player].value().y_speed = 0;
                        break;
                    case sf::Keyboard::Up:
                        velocity[_reg._player].value().x_speed = 0;
                        velocity[_reg._player].value().y_speed = -2;
                        break;
                    case sf::Keyboard::Down:
                        velocity[_reg._player].value().x_speed = 0;
                        velocity[_reg._player].value().y_speed = 2;
                        break;
                    case sf::Keyboard::Space:
                        _reg._eventManager.addEvent<shoot>(_reg._player);
                        break;
                    default:
                        break;
                }
            };
        };
    private:
        registry &_reg;
};

#endif /* !PLAYERSYSTEM_HPP_ */
