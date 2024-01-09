/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** EnnemySystem
*/

#ifndef ENNEMYSYSTEM_HPP_
    #define ENNEMYSYSTEM_HPP_

    #include "../registry/registry.hpp"
    #include "../event/ennemy.hpp"
    #include <random>

class EnnemySystem : public ISystem {
    public:
        EnnemySystem() = delete;
        EnnemySystem(registry &reg): _reg(reg) {};
        ~EnnemySystem() = default;
        void operator()() override {
            for (auto &tmp : _reg._eventManager.getEvent<ennemy>()) {
                EnnemyMove(_reg.addEntity(), tmp->_player_x, tmp->_player_y);
            };
        };
    private:
        void EnnemyMove(uint32_t Entity_id, uint32_t player_x, uint32_t player_y) {
            auto &velocity = _reg.getComponent<Velocity>();
            auto& sprite = _reg.getComponent<Sprite>();
            auto& position = _reg.getComponent<Position>();
            auto &size = _reg.getComponent<Size>();
            auto &collision = _reg.getComponent<CollisionGroup>();
            collision.emplace_at(Entity_id, 3);
            sprite.emplace_at(Entity_id, 3, 0, 0, 576, 430);
            position.emplace_at(Entity_id, 450, (rand() % 900) - 200);
            size.emplace_at(Entity_id, 1, 1);
            velocity.emplace_at(Entity_id, 0, 0, 0, -5, 0);
        }
        registry &_reg;
};

#endif /* !ENNEMYSYSTEM_HPP_ */