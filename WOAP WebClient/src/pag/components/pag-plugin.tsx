import * as React from "react";
import * as PropTypes from "prop-types";
import {Utility} from "../../utility";
import PluginModel, {TrackPluginModelListener} from "../../models/track-plugin-model";
import {ConfigurationManager} from "../../configuration-manager";
import PAGPluginNameLabel from "./pag-plugin-name-label";
import {PAGPluginActiveButton, PAGPluginBypassButton} from "./pag-plugin-control-buttons";
import {ClickableComponent} from "../../components/clickable-component";

interface PAGPluginViewProps {
    pluginModel: PluginModel;
    closeCallback?: () => void;
}

export interface PAGPluginViewState {
    layout: React.ReactElement<any>;
}

export default class PAGPlugin extends ClickableComponent<PAGPluginViewProps, PAGPluginViewState> implements TrackPluginModelListener {
    public static childContextTypes = {
        pluginModel: PropTypes.instanceOf(PluginModel)
    };

    static defaultProps = {
        pluginModel: null,
        closeCallback: () => {
        }
    };

    constructor(props) {
        super(props);
        this.state = {
            layout: null
        };
    }

    public getChildContext() {
        return {
            pluginModel: this.props.pluginModel
        };
    }

    public componentWillMount() {
        this.props.pluginModel.addListener(this);

        let pluginModel = this.props.pluginModel;
        pluginModel.interestedInParameterValues().then(() => {
            if (pluginModel.hasViews()) {
                this.initializeView();
            } else {
                window.WOAP.ServiceLocator.getPAGManager()
                    .prepareModelToPresent(pluginModel)
                    .then(this.initializeView.bind(this));
            }
        });
    }

    public componentWillUnmount() {
        this.props.pluginModel.removeListener(this);
        this.props.pluginModel.noLongerInterestedInParameterValues();
        this.props.pluginModel.pluginViewWillBeDeleted();

        Utility.domRemoveElement("pag-" + this.props.pluginModel.getId());
    }

    public generateErrorLayout(): React.ReactElement<any> {
        return <div className="pluginLoadingError">Error occurred when loading plugin view</div>;
    }

    public nameChanged(newName: string): void {
    }

    public bypassChanged(newState: boolean): void {
    }

    public activeChanged(newState: boolean): void {
    }

    public pluginWillBeDeleted(): void {
        window.WOAP.ServiceLocator.getPAGManager().closePluginModal(this.props.pluginModel);
    }

    public clicked(): void {
        this.props.closeCallback();
    }

    public render() {
        return (
            <div className="pagPlugin">
                <div className="pagPluginTopBar">
                    <div className="pagPluginControlButtons">
                        <PAGPluginActiveButton model={this.props.pluginModel}/>
                        <PAGPluginBypassButton model={this.props.pluginModel}/>
                    </div>
                    <PAGPluginNameLabel model={this.props.pluginModel}/>
                    <div className="pagPluginCloseButton" {...this.getClickableEventProps()}>
                        <svg width={12} height={12}>
                            <line x1={0} y1={0} x2={12} y2={12} style={{strokeWidth: 2}}/>
                            <line x1={0} y1={12} x2={12} y2={0} style={{strokeWidth: 2}}/>
                        </svg>
                    </div>
                </div>
                {this.state.layout}
            </div>);
    }

    protected initializeView() {
        let pluginModel = this.props.pluginModel;
        let view = pluginModel.getDefaultView();

        if (view === null) {
            this.setState({layout: this.generateErrorLayout()});
        } else {
            try {
                let layout = window.WOAP.ServiceLocator.getPAGManager().compileView(view, pluginModel);
                this.setState({layout: layout});
            } catch (error) {
                if (ConfigurationManager.isDebug()) {
                    console.error("Can\"t create view: " + error);
                }
                this.setState({layout: this.generateErrorLayout()});
                return;
            }
        }

        let script = pluginModel.getScript();
        let pluginId = pluginModel.getId();
        if (script.length > 0) {
            let wrappedScript =
                "var pluginModel = window.WOAP.PAGManager.getPluginById(\"" + pluginModel.getTrack().getId() + "\", \"" + pluginId + "\");\n"
                + script;
            Utility.domAddScriptInClosure(wrappedScript, "pag-" + pluginId);
        }

        pluginModel.pluginViewCreated();
    }
}
