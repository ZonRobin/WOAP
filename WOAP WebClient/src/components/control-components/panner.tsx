import * as React from "react";
import {default as Slider, SliderProps, SliderState} from "./slider";

interface PannerState extends SliderState {

}

interface PannerProps extends SliderProps {

}

export default class Panner extends Slider<PannerProps, PannerState> {
    public getWidth(): number {
        return 80;
    }

    public getHeight(): number {
        return 30;
    }

    public render() {
        let normalizedValue = this.valueToNormalizedValue(this.props.value) * 100;

        let valuePath;
        if (normalizedValue <= 50)
            valuePath = <div className="valuePath valuePathLeft"
                             style={{left: normalizedValue + "%", width: (50 - normalizedValue) + "%"}}/>;
        else
            valuePath = <div className="valuePath valuePathRight"
                             style={{width: (normalizedValue - 50) + "%"}}/>;

        return (
            <div className="slider panner"
                 ref={this.boundOnRef}
                 data-scroll-ignore
            >
                {valuePath}
                <div className="midLine"/>
            </div>);
    }
}