////////////////////////////////////////////////////
// This file is part of NoLifeStory.              //
// Please see SuperGlobal.h for more information. //
////////////////////////////////////////////////////
#include "Global.h"

array<NLS::AniSprite, NLS::Mouse::Total> Sprites;

int NLS::Mouse::x = 0;
int NLS::Mouse::y = 0;
NLS::Mouse::StateEnum NLS::Mouse::State = NLS::Mouse::Normal;
NLS::UI::Element* NLS::Mouse::over = nullptr;
int NLS::Mouse::cx = 0;
int NLS::Mouse::cy = 0;

void NLS::Mouse::Init() {
	Node base = WZ["UI"]["Basic"]["Cursor"];
	for (int i = 0; i < Total; ++i) {
		Sprites[i].Set(base[i]);
	}
	State = Normal;
}

void NLS::Mouse::Draw() {
	sf::Vector2i p = sf::Mouse::GetPosition(*window);
	x = p.x;
	y = p.y;

	Sprites[State].Step();
	Sprites[State].Draw(x, y);

	over = nullptr;
	for_each(UI::Window::begin(), UI::Window::end(), [](UI::Window* w) {
		if (x > w->x and x < w->x+w->width and y > w->y and y < w->y+w->height) {
			for_each(w->Elements.begin(), w->Elements.end(), [](UI::Element* e) {
				if (x > e->CalcX() and x < e->CalcX()+e->width and y > e->CalcY() and y < e->CalcY()+e->height) {
					over = e;
				}
			});
		}
	});
	if(State != OnOverClickableLocked) {
		bool found = false;
		for_each(NLS::Life::Npcs.begin(), NLS::Life::Npcs.end(), [&found](pair<uint32_t, NLS::Npc*> n) {
			if(n.second->CheckPosition(View::x+x,View::y+y)) {
				if (sf::Mouse::IsButtonPressed(sf::Mouse::Left)) {
					State = OnOverClickableLocked;
					n.second->MouseFly();
				}
				else {
					State = OnOverClickable;
				}
				found = true;
			}
		});
		if (!found) {
			for_each(NLS::Life::Mobs.begin(), NLS::Life::Mobs.end(), [&found](pair<uint32_t, NLS::Mob*> n) {
				if(!found && n.second->CheckPosition(View::x+x,View::y+y)) {
					if (sf::Mouse::IsButtonPressed(sf::Mouse::Left)) {
						State = OnOverClickableLocked;
						n.second->MouseFly();
						found = true;
					}
				}
			});
		}
		if (!found) {
			State = Normal;
		}
	}
}

void NLS::Mouse::HandleEvent(sf::Event& e) {
	switch (e.Type) {
	case sf::Event::MouseButtonPressed:
		State = OnOverClickableLocked;
		UI::TextBox::Active = nullptr;
		for_each(UI::Window::All.rbegin(), UI::Window::All.rend(), [&](UI::Window* w) {
			if (x > w->x and x < w->x+w->width and y > w->y and y < w->y+w->height) {
				w->Focus();
				sf::Mouse::Button button = e.MouseButton.Button;
				for_each(w->Elements.rbegin(), w->Elements.rend(), [&](UI::Element* m) {
					if (x > m->CalcX() and x < m->CalcX()+m->width and y > m->CalcY() and y < m->CalcY()+m->height) {
						m->Click(button);
					}
				});
			}
		});
		break;
	case sf::Event::MouseButtonReleased:
		State = Normal;
		break;
	case sf::Event::MouseMoved:
		break;
	}
}