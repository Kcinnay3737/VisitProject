#include "Gallery/ButtonGallery.h"
#include "Gallery/Gallery.h"

void UButtonGallery::Initialise(AGallery* Gallery, int Index)
{
	_Gallery = Gallery;
	_Index = Index;
}

void UButtonGallery::Execute()
{
	if (!_Gallery) return;
	_Gallery->OnImageSelected(_Index);
}