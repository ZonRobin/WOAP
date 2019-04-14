import * as React from "react";
import {Utility} from "../../utility";
import Position = Utility.Position;

export interface SliderState {

}

export interface SliderProps {
    value: number;
    valueChangedCallback: (value: number) => any;
    dragStartCallback?: () => any;
    dragEndCallback?: () => any;
    isHorizontal?: boolean;
    dragEasing?: (value: number) => number;
    minValue?: number;
    maxValue?: number;
    skewFactor?: number;
}

export default abstract class Slider<Props extends SliderProps, State extends SliderState> extends React.Component<Props, State> {
    static defaultProps = {
        value: 0,
        valueChangedCallback: (value: number) => {
        },
        dragStartCallback: () => {
        },
        dragEndCallback: () => {
        },
        isHorizontal: false,
        dragEasing: (value: number) => {
            return value / 1.5;
        },
        minValue: 0,
        maxValue: 1,
        skewFactor: 1
    };

    protected isDragging: boolean = false;
    protected boundStartDrag;
    protected boundDragCallback;
    protected boundEndDrag;
    protected boundOnRef;

    protected dragStartTouchPosition: Position = new Position();
    protected dragStartNormalizedValue = 0;

    constructor(props: Props) {
        super(props);

        this.boundStartDrag = this.startDrag.bind(this);
        this.boundDragCallback = this.dragCallback.bind(this);
        this.boundEndDrag = this.endDrag.bind(this);
        this.boundOnRef = this.onRef.bind(this);
    }

    /**
     * Converts 1-0 value to min-max value with skew
     * @param {number} value
     * @returns {number}
     */
    protected normalizedValueToValue(value: number): number {
        if (value === 0)
            return this.props.minValue;

        if (this.props.skewFactor === 1)
            return this.props.minValue + (this.props.maxValue - this.props.minValue) * value;
        else
            return this.props.minValue + (this.props.maxValue - this.props.minValue) * Math.exp(Math.log(value) / this.props.skewFactor);
    }

    /**
     * Converts min-max with skew value to 1-0 value
     * @param {number} rangedValue
     * @returns {number}
     */
    protected valueToNormalizedValue(rangedValue: number): number {
        if (rangedValue < this.props.minValue)
            return 0;

        if (rangedValue > this.props.maxValue)
            return 1;

        if (this.props.skewFactor === 1)
            return (rangedValue - this.props.minValue) / (this.props.maxValue - this.props.minValue);
        else
            return Math.pow((rangedValue - this.props.minValue) / (this.props.maxValue - this.props.minValue), this.props.skewFactor);
    }

    protected startDrag(event) {
        if (event.button !== Utility.LeftMouseButton && event.type !== "touchstart") {
            return;
        }

        event.preventDefault();

        Utility.addEventListener(window, "touchend", this.boundEndDrag, {passive: false, capture: true});
        Utility.addEventListener(window, "touchcancel", this.boundEndDrag, {passive: false, capture: true});
        Utility.addEventListener(window, "touchmove", this.boundDragCallback);
        Utility.addEventListener(window, "mouseup", this.boundEndDrag, {passive: false, capture: true});
        Utility.addEventListener(window, "mousemove", this.boundDragCallback);

        this.dragStartTouchPosition = Utility.getTouchPosition(event);
        this.dragStartNormalizedValue = this.valueToNormalizedValue(this.props.value);
        this.isDragging = true;

        this.props.dragStartCallback();
    }

    protected dragCallback(event) {
        if (!this.isDragging) {
            return;
        }

        let change;
        if (this.props.isHorizontal) {
            change = (Utility.getTouchPosition(event).x - this.dragStartTouchPosition.x) / this.getWidth();
        } else {
            change = (this.dragStartTouchPosition.y - Utility.getTouchPosition(event).y) / this.getHeight();
        }

        let newValue = Utility.inRange(0, 1, this.props.dragEasing(change) + this.dragStartNormalizedValue);
        this.props.valueChangedCallback(this.normalizedValueToValue(newValue));
    }

    protected endDrag(event) {
        event.preventDefault();

        Utility.removeEventListener(window, "touchend", this.boundEndDrag);
        Utility.removeEventListener(window, "touchcancel", this.boundEndDrag);
        Utility.removeEventListener(window, "touchmove", this.boundDragCallback);
        Utility.removeEventListener(window, "mouseup", this.boundEndDrag);
        Utility.removeEventListener(window, "mousemove", this.boundDragCallback);

        this.isDragging = false;
        this.props.dragEndCallback();
    }

    public shouldComponentUpdate(newProps: Props, newState: State) {
        return newProps.value !== this.props.value;
    }

    public onRef(element: Element): void {
        Utility.addEventListener(element, "touchstart", this.boundStartDrag, {passive: false, capture: true});
        Utility.addEventListener(element, "mousedown", this.boundStartDrag, {passive: false, capture: true});
    }

    public abstract getWidth(): number;
    public abstract getHeight(): number;
}