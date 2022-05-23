# https://online-barcode-reader.inliteresearch.com/
# http://zbar.sourceforge.net/
# pip3 install zbarlight
# Has been replaced by image.

class LazyImport:
    def __init__(self, module_name):
        self.module_name = module_name
        self.module = None
    def __getattr__(self, name):
        if self.module is None:
            self.module = __import__(self.module_name)
        return getattr(self.module, name)

zbarlight = LazyImport("zbarlight")

def scan_codes(code_types, img):
    """
    Get *code_type* codes from a maix_image.
    *code_type* can be any of zbar supported code type [#zbar_symbologies]_:
    - **EAN/UPC**: EAN-13 (`ean13`), UPC-A (`upca`), EAN-8 (`ean8`) and UPC-E (`upce`)
    - **Linear barcode**: Code 128 (`code128`), Code 93 (`code93`), Code 39 (`code39`), Interleaved 2 of 5 (`i25`),
    DataBar (`databar`) and DataBar Expanded (`databar-exp`)
    - **2D**: QR Code (`qrcode`)
    - **Undocumented**: `ean5`, `ean2`, `composite`, `isbn13`, `isbn10`, `codabar`, `pdf417`
    .. [#zbar_symbologies] http://zbar.sourceforge.net/iphone/userguide/symbologies.html
    Args:
        code_types (list(str)): Code type(s) to search (see ``zbarlight.Symbologies`` for supported values).
        image (maix_image): Image to scan
    returns:
        A list of *code_type* code values or None
    """
    try:
        if isinstance(code_types, str):
            import warnings
            code_types = [code_types]
            warnings.warn(
                'Using a str for code_types is deprecated, please use a list of str instead',
                DeprecationWarning,
            )
            from zbarlight import _zbarlight
            # Translate symbologies
            symbologies = [
                _zbarlight.Symbologies.get(code_type.upper())
                for code_type in set(code_types)
            ]
            # Check that all symbologies are known
            if None in symbologies:
                bad_code_types = [code_type for code_type in code_types if code_type.upper() not in _zbarlight.Symbologies]
                class UnknownSymbologieError(Exception):
                    pass
                raise UnknownSymbologieError('Unknown Symbologies: %s' % bad_code_types)
            # Convert the image to be used by c-extension
            from maix import image
            if not isinstance(img, image.Image):
                raise RuntimeError('Bad or unknown image type')
            return _zbarlight.zbar_code_scanner(symbologies, img.copy().convert('L').tobytes(), img.width, img.height)
    except ModuleNotFoundError as e:
        print(e)
    return None

if __name__ == '__main__':
    from maix import camera, display, zbar
    while True:
        tmp = camera.capture()
        result = zbar.scan_codes(["qrcode", "code39"], tmp)
        print(str(result))
        display.show(tmp.draw_string(10, 10, str(result), 2.0, (255, 0, 0)))
