/**
 * @file Timestep.hpp
 *
 * @copyright Copyright (c) 2024 Tycjan Fortuna
 */
#pragma once

namespace SW::Windowing
{

	class Timestep
	{
	public:
		// Constructs a Timestep object with the specified time (The time interval is in seconds).
		Timestep(float time = 0.0f) : m_Time(time) {}

		// The time interval in seconds.
		operator float() const { return m_Time; }

		[[nodiscard]] float GetSeconds() const { return m_Time; }
		[[nodiscard]] float GetMilliseconds() const { return m_Time * 1000.0f; }

	private:
		float m_Time; // in seconds.
	};

} // namespace SW::Windowing
