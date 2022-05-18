#include "spectrum.h"

#include "imgui.h"

namespace ImGui {
    namespace Spectrum {
       // extern const unsigned int SourceSansProRegular_compressed_size = 149392;
       // extern const unsigned int SourceSansProRegular_compressed_data[]; // defined later in the file

        void StyleColorsSpectrum() {
            ImGuiIO& io = ImGui::GetIO();
           // ImFont* font = io.Fonts->AddFontFromMemoryCompressedTTF(OpenSans_data, OpenSans_size, 16.0f);
           // if (font) io.FontDefault = font;

            ImGuiStyle* style = &ImGui::GetStyle();
            style->GrabRounding = 4.0f;

            ImVec4* colors = style->Colors;
            colors[ ImGuiCol_TextDisabled ] = ColorConvertU32ToFloat4( Spectrum::GRAY500 );

            colors[ ImGuiCol_MenuBarBg ] = ColorConvertU32ToFloat4( Spectrum::GRAY100 );
            colors[ ImGuiCol_Header ] = ColorConvertU32ToFloat4( Spectrum::BLUE400 );
            colors[ ImGuiCol_HeaderHovered ] = ColorConvertU32ToFloat4( Spectrum::BLUE500 );
            colors[ ImGuiCol_HeaderActive ] = ColorConvertU32ToFloat4( Spectrum::BLUE600 );

            colors[ ImGuiCol_PlotLines ] = ColorConvertU32ToFloat4( Spectrum::BLUE400 );
            colors[ ImGuiCol_PlotLinesHovered ] = ColorConvertU32ToFloat4( Spectrum::BLUE600 );
            colors[ ImGuiCol_PlotHistogram ] = ColorConvertU32ToFloat4( Spectrum::BLUE400 );
            colors[ ImGuiCol_PlotHistogramHovered ] = ColorConvertU32ToFloat4( Spectrum::BLUE600 );

            colors[ ImGuiCol_TextSelectedBg ] = ColorConvertU32ToFloat4( ( Spectrum::BLUE400 & 0x00FFFFFF ) | 0x33000000 );

            colors[ ImGuiCol_DragDropTarget ] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );

            colors[ ImGuiCol_NavHighlight ] = ColorConvertU32ToFloat4( ( Spectrum::GRAY900 & 0x00FFFFFF ) | 0x0A000000 );
            colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
            colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
            colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
        }

    }
}
