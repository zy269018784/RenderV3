#pragma once
#include <Math/Math.h> 
#include <Util/TaggedPointer.h>
#include <Texture/TextureEvalContext.h>
#include <Intersection/SurfaceInteraction.h>
#include <Intersection/Interaction.h>

namespace Render {

	// TexCoord2D Definition
	struct TexCoord2D {
		Point2f st;
		Float dsdx, dsdy, dtdx, dtdy;
		//std::string ToString() const;
	};



    class UVMapping;
    class TextureMapping2D : public TaggedPointer<UVMapping> {
    public:
        // TextureMapping2D Interface
        using TaggedPointer::TaggedPointer;


        CPU_GPU inline TexCoord2D Map(TextureEvalContext ctx) const;
    };

}