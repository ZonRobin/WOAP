import * as React from "react";
import {Utility} from "../../utility";
import Position = Utility.Position;
import ResizeWatcherListener = Utility.ResizeWatcherListener;
import {default as Slider, SliderProps, SliderState} from "./slider";

interface LinearSliderState extends SliderState {

}

interface LinearSliderProps extends SliderProps {

}

export default class LinearSlider extends Slider<LinearSliderProps, LinearSliderState> implements ResizeWatcherListener {

    protected sliderElement: Element = null;
    protected sliderHandle: Element = null;

    protected onRefSliderElementBound;
    protected onRefSliderHandleBound;

    public constructor(props) {
        super(props);

        this.onRefSliderElementBound = this.onRefSliderElement.bind(this);
        this.onRefSliderHandleBound = this.onRefSliderHandle.bind(this);
    }

    public countHandlePosition(): Position {
        if (this.sliderElement === null)
            return new Position();

        if (this.props.isHorizontal) {
            return new Position((this.sliderElement.clientWidth - this.sliderHandle.clientWidth) * (1 - this.valueToNormalizedValue(this.props.value)), 0);
        }
        else {
            return new Position(0, (this.sliderElement.clientHeight - this.sliderHandle.clientHeight) * (1 - this.valueToNormalizedValue(this.props.value)));
        }
    }

    public componentWillMount() {
        window.WOAP.ResizeWatcher.addListener(this);
    }

    public componentWillUnmount() {
        window.WOAP.ResizeWatcher.removeListener(this);
    }

    public componentDidMount() {
        this.forceUpdate();
    }

    public getWidth(): number {
        return this.sliderElement.clientWidth;
    }

    public getHeight(): number {
        return this.sliderElement.clientHeight;
    }

    public documentResized(event, orientationChanged: boolean) {
        this.forceUpdate();
    }

    public onRefSliderElement(e) {
        this.sliderElement = e;
    }

    public onRefSliderHandle(e) {
        this.sliderHandle = e;
        this.boundOnRef(e);
    }

    public render() {
        let handlePosition = this.countHandlePosition();

        return (
            <div className="slider linearFader" data-scroll-ignore ref={this.onRefSliderElementBound}>
                <div className="sliderTrail"/>
                <div className="sliderHandle"
                     ref={this.onRefSliderHandleBound}
                     style={{left: handlePosition.x, top: handlePosition.y}}
                >
                    {this.props.children}
                </div>
            </div>);
    }
}