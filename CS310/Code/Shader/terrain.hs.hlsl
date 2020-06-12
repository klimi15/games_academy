// TERRAIN_MODE=2

#include "terrain_shader_types.hpp"

cbuffer c_constants : register( b0 )
{
	TerrainHullConstants c_terrain;
};

PatchConstantOutput PatchConstantFunction( InputPatch< VertexInputOutput, 4 > patches, uint patchId : SV_PrimitiveID )
{
	float2 cameraPosition	= getTerrainHullCameraPosition( c_terrain ).xz;
	float constantFactor	= getTerrainHullTessellationFactor( c_terrain );

	float tessellationAmount[ 4 ];
	[unroll]
	for( uint i = 0; i < 4; i++ )
	{
		float2 position				= patches[ i ].position.xz;
		float cameraDistance		= distance( cameraPosition, position );
		float distanceFactor		= max( (2400 - cameraDistance) / 2400, 0.0 );

		tessellationAmount[ i ] = max( 1.0, constantFactor * distanceFactor * distanceFactor * distanceFactor * 120.0 );
	}

	// Order
	// corner = patch index
	// line = edge index
	// 0---0---2
	// |       |
	// 1       3
	// |       |
	// 1---2---3

	PatchConstantOutput output;
	output.edges[ 0 ] = tessellationAmount[ 0 ] + tessellationAmount[ 2 ];
	output.edges[ 1 ] = tessellationAmount[ 0 ] + tessellationAmount[ 1 ];
	output.edges[ 2 ] = tessellationAmount[ 1 ] + tessellationAmount[ 3 ];
	output.edges[ 3 ] = tessellationAmount[ 2 ] + tessellationAmount[ 3 ];

	output.inside[ 0 ] = (output.edges[ 0 ] + output.edges[ 3 ]) / 2.0;
	output.inside[ 1 ] = (output.edges[ 1 ] + output.edges[ 2 ]) / 2.0;

	return output;
}

[domain( "quad" )]
[partitioning( "integer" )]
[outputtopology( "triangle_ccw" )]
[outputcontrolpoints( 4 )]
[patchconstantfunc( "PatchConstantFunction" )]
HullOutput main( InputPatch< VertexInputOutput, 4 > patches, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID )
{
	HullOutput output;
	output.position	= patches[ pointId ].position;
	output.uv		= patches[ pointId ].uv;
#if TERRAIN_MODE != TERRAIN_MODE_SHADOW

	if( pointId == 0 )
	{
		output.color = float3(1, 0, 0);
	}
	else if( pointId == 1 )
	{
		output.color = float3(0, 1, 0);
	}
	else if( pointId == 2 )
	{
		output.color = float3(0, 0, 1);
	}
	else //if( pointId == 3 )
	{
		output.color = float3(1, 1, 0);
	}
#endif

	return output;
}
