import * as React from "react";
import Scrollable from "./scrollable";
import LoadingSpinner from "./loading-spinner";

interface ContextMenuContentProps {
    className?: string;
    isHorizontal?: boolean;
    showLoadingSpinner?: boolean;
    backgroundColor?: string;
}

interface ContextMenuContentState {
    maxLength: string;
    margin: number;
}

export default class ContextMenuContent extends React.Component<ContextMenuContentProps, ContextMenuContentState> {

    static defaultProps = {
        className: "",
        isHorizontal: false,
        showLoadingSpinner: false,
        backgroundColor: ""
    };

    protected menuElement: HTMLElement;
    protected contexifyElement: HTMLElement;
    protected onRefBound;
    protected lastPosition: number = -1;
    protected stopPropagationBound;

    public constructor(props) {
        super(props);

        this.state = {
            maxLength: (props.isHorizontal ? "100vw" : "100vh"),
            margin: 0
        };

        this.onRefBound = this.onRef.bind(this);
        this.stopPropagationBound = this.stopPropagation.bind(this);
    }

    public componentDidMount() {
        window.WOAP.ContextMenuManager.contextMenuWasOpened();
    }

    public componentWillUnmount() {
        window.WOAP.ContextMenuManager.contextMenuWasClosed();
    }

    public onRef(element: HTMLElement) {
        this.menuElement = element;

        if (element === null)
            return;

        this.contexifyElement = element;
        while (this.contexifyElement !== null) {
            if (this.contexifyElement.className.indexOf("react-contexify") !== -1) {
                break;
            }

            this.contexifyElement = this.contexifyElement.parentElement;
        }

        this.recountLength();
    }

    public recountLength() {
        if (this.contexifyElement === null)
            return;

        if (this.props.isHorizontal) {
            let newPosition = parseInt(this.contexifyElement.style.left, 10);
            if (newPosition !== this.lastPosition) {
                this.lastPosition = newPosition;

                if (newPosition < 0) {
                    this.setState({
                        maxLength: "calc(100vw - 20px)",
                        margin: Math.abs(newPosition)
                    });
                } else {
                    this.setState({
                        maxLength: "calc(100vw - " + (newPosition + 20) + "px)",
                        margin: 0
                    });
                }
            }
        } else {
            let newPosition = parseInt(this.contexifyElement.style.top, 10);
            if (newPosition !== this.lastPosition) {
                this.lastPosition = newPosition;

                if (newPosition < 0) {
                    this.setState({
                        maxLength: "calc(100vh - 20px)",
                        margin: Math.abs(newPosition)
                    });
                } else {
                    this.setState({
                        maxLength: "calc(100vh - " + (newPosition + 20) + "px)",
                        margin: 0
                    });
                }
            }
        }
    }

    public componentDidUpdate() {
        this.recountLength();
    }

    public stopPropagation(ev) {
        ev.stopPropagation();
    }

    public render() {
        let style = {
            marginLeft: this.props.isHorizontal ? this.state.margin : 0,
            marginTop: this.props.isHorizontal ? 0 : this.state.margin,
            backgroundColor: this.props.backgroundColor
        };

        return (
            <div className={"contextMenu " + this.props.className}
                 onMouseEnter={this.stopPropagationBound}
                 onClick={this.stopPropagationBound}
                 onMouseDown={this.stopPropagationBound}
                 ref={this.onRefBound}
                 style={style}
            >
                <Scrollable className="contextMenuList"
                            maxLength={this.state.maxLength}
                            isHorizontal={this.props.isHorizontal}
                >
                    {this.props.children}
                </Scrollable>
                {this.props.showLoadingSpinner ? <LoadingSpinner overlay={true}/> : []}
            </div>
        );
    }
}
