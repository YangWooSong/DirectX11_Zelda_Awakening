#pragma once

#include <process.h>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum EFFECT_TYPE { MONSTER_DIED_EFFECT, BOMB_FUSE_EFFECT, BOMB_EXPLOSION_EFFECT, PLAYER_ITEM_GET_EFFECT, MONSTER_HIT_EFFECT, PLAYER_CHARGE_SLASH_EFFECT, 
		TAIL_DUNGEON_OPEN, BOX_APPEAR, SPARK_EFFECT, PAWN_HIT_EFFECT, VEAGAS_HIT_EFFECT,FIRE_BIG_EFFECT,LOCKBLOCK_OPEN, ROLA_HIT_EFFECT, TAIL_HIT_EFFECT, WINDOW_LIGHT, 
		PLAYER_FOOT, ROLA_HAND, TYPE_END };
	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_MARINHOUSE, LEVEL_FIELD, LEVEL_DUNGEON, LEVEL_STORE, LEVEL_TEST, LEVEL_END };
	enum SOUNDID{SOUND_BGM, SOUND_PLAYER, SOUND_PLAYER_EFFECT, SOUND_WEAPON, SOUND_MONSTER, SOUND_END};
	enum aiTextureType
	{
		/** Dummy value.
		 *
		 *  No texture, but the value to be used as 'texture semantic'
		 *  (#aiMaterialProperty::mSemantic) for all material properties
		 *  *not* related to textures.
		 */
		aiTextureType_NONE = 0,

		/** LEGACY API MATERIALS
		 * Legacy refers to materials which
		 * Were originally implemented in the specifications around 2000.
		 * These must never be removed, as most engines support them.
		 */

		 /** The texture is combined with the result of the diffuse
		  *  lighting equation.
		  */
		aiTextureType_DIFFUSE = 1,

		/** The texture is combined with the result of the specular
		 *  lighting equation.
		 */
		aiTextureType_SPECULAR = 2,

		/** The texture is combined with the result of the ambient
		 *  lighting equation.
		 */
		aiTextureType_AMBIENT = 3,

		/** The texture is added to the result of the lighting
		 *  calculation. It isn't influenced by incoming light.
		 */
		aiTextureType_EMISSIVE = 4,

		/** The texture is a height map.
		 *
		 *  By convention, higher gray-scale values stand for
		 *  higher elevations from the base height.
		 */
		aiTextureType_HEIGHT = 5,

		/** The texture is a (tangent space) normal-map.
		 *
		 *  Again, there are several conventions for tangent-space
		 *  normal maps. Assimp does (intentionally) not
		 *  distinguish here.
		 */
		aiTextureType_NORMALS = 6,

		/** The texture defines the glossiness of the material.
		 *
		 *  The glossiness is in fact the exponent of the specular
		 *  (phong) lighting equation. Usually there is a conversion
		 *  function defined to map the linear color values in the
		 *  texture to a suitable exponent. Have fun.
		*/
		aiTextureType_SHININESS = 7,

		/** The texture defines per-pixel opacity.
		 *
		 *  Usually 'white' means opaque and 'black' means
		 *  'transparency'. Or quite the opposite. Have fun.
		*/
		aiTextureType_OPACITY = 8,

		/** Displacement texture
		 *
		 *  The exact purpose and format is application-dependent.
		 *  Higher color values stand for higher vertex displacements.
		*/
		aiTextureType_DISPLACEMENT = 9,

		/** Lightmap texture (aka Ambient Occlusion)
		 *
		 *  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
		 *  covered by this material property. The texture contains a
		 *  scaling value for the final color value of a pixel. Its
		 *  intensity is not affected by incoming light.
		*/
		aiTextureType_LIGHTMAP = 10,

		/** Reflection texture
		 *
		 * Contains the color of a perfect mirror reflection.
		 * Rarely used, almost never for real-time applications.
		*/
		aiTextureType_REFLECTION = 11,

		/** PBR Materials
		 * PBR definitions from maya and other modelling packages now use this standard.
		 * This was originally introduced around 2012.
		 * Support for this is in game engines like Godot, Unreal or Unity3D.
		 * Modelling packages which use this are very common now.
		 */

		aiTextureType_BASE_COLOR = 12,
		aiTextureType_NORMAL_CAMERA = 13,
		aiTextureType_EMISSION_COLOR = 14,
		aiTextureType_METALNESS = 15,
		aiTextureType_DIFFUSE_ROUGHNESS = 16,
		aiTextureType_AMBIENT_OCCLUSION = 17,

		/** Unknown texture
		 *
		 *  A texture reference that does not match any of the definitions
		 *  above is considered to be 'unknown'. It is still imported,
		 *  but is excluded from any further post-processing.
		*/
		aiTextureType_UNKNOWN = 18,


#ifndef SWIG
		_aiTextureType_Force32Bit = INT_MAX
#endif
	};
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;
