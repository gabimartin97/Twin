#include "Shia.h"
#include "SpriteEffect.h"
#include "World.h"

Shia::Shia( const Vec2& pos )
	:
	Entity( pos,75.0f,90.0f,60.0f )
{}

void Shia::ProcessLogic( const World& world )
{
	pBrainState->ProcessLogic( *this,world );
}

void Shia::Update( World& world,float dt )
{
	pos += vel * dt;

	if( isDoingBoundaryAdjustment )
	{
		world.GetBoundsConst().Adjust( *this );
	}

	if( effectActive )
	{
		effectTime += dt;
		if( effectTime >= effectDuration )
		{
			effectActive = false;
		}
	}

	// handle brain state transition / update
	// delete old state if new one is returned
	if( auto pNewState = pBrainState->Update( *this,world,dt ) )
	{
		delete pBrainState;
		pBrainState = pNewState;
	}
}

void Shia::ApplyDamage( float damage )
{
	effectActive = true;
	effectTime = 0.0f;
}

void Shia::Draw( Graphics& gfx ) const
{
	// calculate drawing base
	const auto draw_pos = Vei2( pos + draw_offset );
	// roomba offset relative to body	
	const auto roomba_pos = Vei2( draw_pos ) + Vei2{ 7,40 };

	if( effectActive ) // draw damage flash
	{
		gfx.DrawSprite( draw_pos.x,draw_pos.y,*pShiaSurf,SpriteEffect::SubstitutionAlpha{ Colors::White },facingLeft );
	}
	else // draw normal
	{
		gfx.DrawSprite( draw_pos.x,draw_pos.y,*pShiaSurf,SpriteEffect::AlphaBlendBaked{},facingLeft );
	}
}

Shia::SlowRollState::SlowRollState( Shia& shia,const Vec2& target )
	:
	target( target )
{
	shia.isDoingBoundaryAdjustment = false;
}

void Shia::SlowRollState::ProcessLogic( Shia& shia,const World& world ) const
{
	shia.SetDirection( (target - shia.GetPos()).GetNormalized() );
}

Shia::BrainState* Shia::SlowRollState::Update( Shia& shia,const World& world,float dt )
{
	if( (target - shia.GetPos()).GetLengthSq() < 6.9f )
	{
		// now we are ready to live our lives in the world
		shia.isDoingBoundaryAdjustment = true;
		return new EaseInto( shia,{ 150.0f,150.0f },400.0f );
	}
	return nullptr;
}
