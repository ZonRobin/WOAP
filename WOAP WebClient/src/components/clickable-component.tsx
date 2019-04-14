import * as React from "react";
import {Utility} from "../utility";

export abstract class ClickableComponent<Props, State> extends React.Component<Props, State> {

    protected buttonDown: boolean = false;
    protected fingerDown: boolean = false;

    protected boundMouseDown;
    protected boundMouseUp;

    protected boundTouchStart;
    protected boundTouchEnd;

    constructor(props) {
        super(props);

        this.boundMouseUp = this.mouseUpCallback.bind(this);
        this.boundMouseDown = this.mouseDownCallback.bind(this);
        this.boundTouchStart = this.touchStartCallback.bind(this);
        this.boundTouchEnd = this.touchEndCallback.bind(this);
    }

    public abstract clicked(): void;

    protected mouseDownCallback(event) {
        this.buttonDown = event.button === Utility.LeftMouseButton;
    }

    protected mouseUpCallback() {
        if (!this.buttonDown || window.WOAP.MouseWatcher.wasTouchEndLately())
            return;

        this.fingerDown = false;
        this.buttonDown = false;

        if (!window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown()
            && !window.WOAP.ContextMenuManager.isContextMenuOpened())
            this.clicked();
    }

    protected touchStartCallback() {
        this.fingerDown = true;
    }

    protected touchEndCallback() {
        if (!this.fingerDown)
            return;

        this.fingerDown = false;
        this.buttonDown = false;

        if (!window.WOAP.MouseWatcher.isMouseDraggedSinceMouseDown()
            && !window.WOAP.ContextMenuManager.isContextMenuOpened())
            this.clicked();
    }

    protected getClickableEventProps() {
        return {
            onTouchStart: this.boundTouchStart,
            onTouchEnd: this.boundTouchEnd,
            onMouseDown: this.boundMouseDown,
            onMouseUp: this.boundMouseUp
        };
    }
}
