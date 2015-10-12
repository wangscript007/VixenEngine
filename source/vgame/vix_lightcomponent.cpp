/*
	Copyright (C) 2015  Matt Guerrette

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vix_lightcomponent.h>
#include <vix_gameobject.h>

namespace Vixen {
	LightComponent::LightComponent(ILight* light)
	{
		m_light = light;
	}

	LightComponent::~LightComponent()
	{
		delete m_light;
	}

	void LightComponent::VOnInit()
	{

	}

	void LightComponent::VOnEnable()
	{

	}

	void LightComponent::VUpdate()
	{

	}

	void LightComponent::VOnDisable()
	{

	}

	void LightComponent::VOnDestroy()
	{

	}

	void LightComponent::VBindParent(GameObject* parent)
	{
		m_parent = parent;
		//m_parentTransform = parent->GetTransform();
	}


}