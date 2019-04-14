import Color from "./color";

export default class ColorHelper {
    public static getBrighterColor(colorToUse: Color, amount: number = 0.4): Color {
        amount = 1 / (1 + amount);

        return new Color({
            red: 255 - (amount * (255 - colorToUse._red)),
            green: 255 - (amount * (255 - colorToUse._green)),
            blue: 255 - (amount * (255 - colorToUse._blue))
        });
    }

    public static getPerceivedBrightness(color: Color): number {
        return 0.2126 * (color._red / 255) + 0.7152 * (color._green / 255) + 0.0722 * (color._blue / 255);
    }
}