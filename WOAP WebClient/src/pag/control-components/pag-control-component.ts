import {ConfigurationManager} from "../../configuration-manager";
import {ArrayExtended} from "../../array-extended";

export interface PAGControlComponentState {

}

export interface PAGControlComponentProps {
    value: number;
    valueChangedCallback: (newValue: number) => void;
    width: number;
    height: number;
    steps?: ArrayExtended<number>;
    inverted?: boolean;
    colorScheme?: PAGControlComponentColorScheme;
    beginGestureCallback?: () => any;
    endGestureCallback?: () => any;
}

export default class PAGControlComponentColorScheme {
    public accent: string;
    public fill: string;

    public constructor(accent: string, fill: string) {
        this.accent = accent;
        this.fill = fill;
    }

    public static findByName(schemeName: string, fallback: string = "grey"): PAGControlComponentColorScheme {
        if (schemeName !== null && schemeName.length > 0) {
            if (PAGControlComponentColorSchemes.hasOwnProperty(schemeName)) {
                return PAGControlComponentColorSchemes[schemeName];
            } else {
                if (ConfigurationManager.isDebug()) {
                    console.error("Color scheme not found: " + schemeName);
                }
                if (!PAGControlComponentColorSchemes.hasOwnProperty(fallback)) {
                    if (ConfigurationManager.isDebug()) {
                        console.error("Fallback color scheme not found: " + fallback);
                    }
                    return this.getDefault();
                } else {
                    return PAGControlComponentColorSchemes[fallback];
                }
            }
        }
    }

    public static getDefault(): PAGControlComponentColorScheme {
        return PAGControlComponentColorSchemes.green;
    }
}

export let PAGControlComponentColorSchemes = {
    red: new PAGControlComponentColorScheme("#E12525", "rgb(33, 33, 33)"),
    orange: new PAGControlComponentColorScheme("#F5A22E", "rgb(33, 33, 33)"),
    yellow: new PAGControlComponentColorScheme("#F0E223", "rgb(33, 33, 33)"),
    lightGreen: new PAGControlComponentColorScheme("#7AD975", "rgb(33, 33, 33)"),
    green: new PAGControlComponentColorScheme("#1FBA46", "rgb(33, 33, 33)"),
    lightBlue: new PAGControlComponentColorScheme("#00C1E8", "rgb(33, 33, 33)"),
    blue: new PAGControlComponentColorScheme("#4967F2", "#AAA"),
    purple: new PAGControlComponentColorScheme("#7239B8", "#AAA"),
    grey: new PAGControlComponentColorScheme("#AAAAAA", "rgb(33, 33, 33)"),
    black: new PAGControlComponentColorScheme("#1E1E1E", "#AAA"),
    white: new PAGControlComponentColorScheme("#DCDCDC", "rgb(33, 33, 33)"),
};