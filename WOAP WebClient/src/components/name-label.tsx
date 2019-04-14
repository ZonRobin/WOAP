import * as React from "react";
import Color from "../helpers/color/color";
import {Textfit} from "react-textfit";
import {Utility} from "../utility";

export interface NameLabelProps {
    text: string;
    editorText?: string;
    backgroundColor?: Color;
    useDarkTextColor?: boolean;
    maxFontSize?: number;
    editable?: boolean;
    fitText?: boolean;
    maxLength?: number;
    textEdited?: (string) => void;
}

export interface NameLabelState {
    editorShown: boolean;
    editorText: string;
}

export default class NameLabel extends React.Component<NameLabelProps, NameLabelState> {
    static defaultProps = {
        text: "",
        editorText: "",
        useDarkTextColor: false,
        backgroundColor: null,
        maxFontSize: 13,
        editable: false,
        fitText: true,
        maxLength: 12,
        textEdited: () => {
        }
    };

    protected mouseUpCallbackBound;
    protected touchEndCallbackBound;
    protected keyDownCallbackBound;
    protected inputBlurCallbackBound;
    protected inputMouseDownCallbackBound;

    protected htmlInputElement: HTMLInputElement = null;
    protected htmlInputRenderTime: number = 0;
    protected lastMouseUp: number = 0;

    public constructor(props) {
        super(props);

        this.state = {
            editorShown: false,
            editorText: this.props.text
        };

        this.mouseUpCallbackBound = this.mouseUpCallback.bind(this);
        this.touchEndCallbackBound = this.touchEndCallback.bind(this);
        this.keyDownCallbackBound = this.keyDownCallback.bind(this);
        this.inputBlurCallbackBound = this.inputBlurCallback.bind(this);
        this.inputMouseDownCallbackBound = this.inputMouseDownCallback.bind(this);
    }

    public mouseUpCallback() {
        if (window.WOAP.MouseWatcher.wasTouchEndLately())
            return;

        this.touchEndCallback();
    }

    public touchEndCallback() {
        let now = new Date().getTime();

        if (now - this.lastMouseUp < 500) {
            this.lastMouseUp = 0;
            this.setState({
                editorShown: true,
                editorText: this.props.text
            });
        } else {
            this.lastMouseUp = now;
        }
    }

    public keyDownCallback(ev) {
        if (ev.keyCode === Utility.EnterKey) {
            this.stopEditing();
        } else if (ev.keyCode === Utility.EscapeKey) {
            this.stopEditing(true);
        }
    }

    public inputBlurCallback() {
        this.stopEditing();
    }

    public inputMouseDownCallback(ev) {
        if (!window.WOAP.MouseWatcher.wasTouchEndLately())
            return;

        if (new Date().getTime() - this.htmlInputRenderTime < 1000)
            ev.preventDefault();
    }

    public htmlInputRendered(element: HTMLInputElement) {
        this.htmlInputElement = element;
        this.htmlInputRenderTime = new Date().getTime();

        if (element !== null)
            element.select();
    }

    public stopEditing(discard: boolean = false) {
        if (!discard) {
            this.props.textEdited(this.htmlInputElement.value);
        }

        this.setState({
            editorShown: false
        });
    }

    public getMouseUpEventProps(): {} {
        if (!this.props.editable)
            return {};

        return {
            onTouchEnd: this.touchEndCallbackBound,
            onMouseUp: this.mouseUpCallbackBound
        };
    }

    public render() {
        return (
            <div className={"nameLabel" + (this.props.useDarkTextColor ? " nameLabelDark" : " nameLabelLight")}
                 style={this.props.backgroundColor !== null ? {backgroundColor: this.props.backgroundColor.getHex()} : {}}
            >
                {!this.state.editorShown
                    ?
                    <div className={"nameLabelWrap" + (this.props.fitText ? " nameLabelFitText" : "")}
                         {...this.getMouseUpEventProps()}
                    >
                        {this.props.fitText
                            ?
                            <Textfit forceSingleModeWidth={false}
                                     mode={"single"}
                                     max={this.props.maxFontSize}
                                     autoResize={false}
                                     throttle={80}
                            >
                                {this.props.text}
                            </Textfit>
                            :
                            <div className="nameLabelText">
                                {this.props.text}
                            </div>
                        }
                    </div>
                    :
                    <input type="text"
                           defaultValue={this.props.editorText.length === 0 ? this.props.text : this.props.editorText}
                           onKeyDown={this.keyDownCallbackBound}
                           onBlur={this.inputBlurCallbackBound}
                           onMouseDown={this.inputMouseDownCallbackBound}
                           ref={(e) => this.htmlInputRendered(e)}
                           maxLength={this.props.maxLength}
                    />
                }
            </div>
        );
    }
}