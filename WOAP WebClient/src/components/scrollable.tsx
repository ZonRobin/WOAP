import * as React from "react";
import {Utility} from "../utility";
import Position = Utility.Position;

export interface ScrollableProps {
    className?: string;
    isHorizontal?: boolean;
    maxLength?: string;
    dragEasing?: (value: number) => number;
    dragStartCallback?: () => any;
    dragEndCallback?: () => any;
}

export default class Scrollable extends React.Component<ScrollableProps, {}> {
    static defaultProps = {
        className: "",
        isHorizontal: false,
        maxLength: "100%",
        dragStartCallback: () => {
        },
        dragEndCallback: () => {
        },
        dragEasing: (value: number) => {
            return value;
        }
    };

    protected wrapperElement: Element = null;
    protected boundStartDrag;
    protected boundDragCallback;
    protected boundEndDrag;

    protected dragStartTouchPosition: Position = new Position();
    protected dragStartScrolledTo: Position = new Position();

    public constructor(props) {
        super(props);

        this.boundStartDrag = this.startDrag.bind(this);
        this.boundDragCallback = this.dragCallback.bind(this);
        this.boundEndDrag = this.endDrag.bind(this);
    }

    public componentWillUnmount() {
        this.endDrag();
    }

    public startDrag(event) {
        if (event.button !== Utility.LeftMouseButton && event.type !== "touchstart") {
            return;
        }

        let elem = event.target;
        while (elem !== null) {
            if (elem.hasAttribute("data-scroll-ignore")) {
                return;
            } else if (elem.hasAttribute("data-scrollable")) {
                if (elem === this.wrapperElement)
                    break;
                else
                    return;
            }

            elem = elem.parentElement;
        }

        Utility.addEventListener(window, "mouseup", this.boundEndDrag);
        Utility.addEventListener(window, "touchend", this.boundEndDrag);
        Utility.addEventListener(window, "touchcancel", this.boundEndDrag);
        Utility.addEventListener(window, "touchmove", this.boundDragCallback);
        Utility.addEventListener(window, "mousemove", this.boundDragCallback);

        this.dragStartTouchPosition = Utility.getTouchPosition(event);
        this.dragStartScrolledTo = new Position(this.wrapperElement.scrollLeft, this.wrapperElement.scrollTop);
    }

    public dragCallback(event) {
        let cursorPosition = Utility.getTouchPosition(event);

        if (this.props.isHorizontal) {
            this.wrapperElement.scrollLeft = Utility.higherOf(0, this.props.dragEasing(this.dragStartTouchPosition.x - cursorPosition.x) + this.dragStartScrolledTo.x);
        } else {
            this.wrapperElement.scrollTop = Utility.higherOf(0, this.props.dragEasing(this.dragStartTouchPosition.y - cursorPosition.y) + this.dragStartScrolledTo.y);
        }
    }

    public endDrag() {
        Utility.removeEventListener(window, "mouseup", this.boundEndDrag);
        Utility.removeEventListener(window, "touchend", this.boundEndDrag);
        Utility.removeEventListener(window, "touchcancel", this.boundEndDrag);
        Utility.removeEventListener(window, "touchmove", this.boundDragCallback);
        Utility.removeEventListener(window, "mousemove", this.boundDragCallback);

        this.props.dragEndCallback();
    }

    public getRootStyle(): {} {
        if (this.props.isHorizontal) {
            return {
                overflow: "hidden",
                maxWidth: this.props.maxLength
            };
        } else {
            return {
                overflow: "hidden",
                maxHeight: this.props.maxLength
            };
        }
    }

    public render() {
        return (
            <div
                className={this.props.className + (this.props.isHorizontal ? " scrollableHorizontal" : " scrollableVertical")}
                onMouseDown={this.boundStartDrag}
                onTouchStart={this.boundStartDrag}
                style={this.getRootStyle()}
            >
                <div ref={(e) => this.wrapperElement = e}
                     style={this.getWrapperStyle()}
                     className="scrollableWrapper"
                     data-scrollable
                >
                    {this.props.children}
                </div>
            </div>
        );
    }

    protected getWrapperStyle(): {} {
        if (this.props.isHorizontal) {
            return {
                height: "calc(100% + " + window.WOAP.ScrollbarsSize.horizontalBarHeight + "px)"
            };
        } else {
            return {
                width: "calc(100% + " + window.WOAP.ScrollbarsSize.verticalBarWidth + "px)"
            };
        }
    }
}