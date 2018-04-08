#pragma once
/*
namespace eng {

	class Input {
	public:
		enum Action : unsigned char {
			FORWARD = 0,
			BACKWARD,
			LEFT,
			RIGHT,
			COUNT,
			INVALID = 255
		};

	public:
		Input();
		~Input();

		void UpdateInput();

		bool Quit() const { return m_quit; }
		bool GetAction(int action) const { return m_keys[action]; }
		void GetMousePos(int& x, int &y) const { x = m_mouseX; y = m_mouseY; }

	private:
		// bool m_keys[KBKey::KB_COUNT];
		bool m_keys[322];

		bool m_quit;

		int m_mouseX;
		int m_mouseY;
	};

}*/