import "./app.css";
import "react-contexify/dist/ReactContexify.min.css";
import 'react-toastify/dist/ReactToastify.min.css';

import * as React from "react";
import APICore, {APICoreConnectionListener, MessageReceiver} from "./api/api-core";
import OSCMessage from "./osc/osc-message";
import OSCBundle from "./osc/osc-bundle";
import PAGManager from "./pag/pag-manager";
import ServiceLocator from "./service-locator";
import StoresManager from "./stores/store-manager";
import {
    ApiCoreErrorMessage,
    ApiCoreGetServerInfoMessage,
    ApiCoreReadyMessage,
    ApiCoreServerInfoMessage
} from "./api/messages/app-messages";
import {ServerInfo, SystemStoreListener} from "./stores/system-store";
import Mixer from "./components/mixer";
import Login from "./login";
import {Route, Switch} from "react-router";
import {History} from "history";
import {toast, ToastContainer} from "react-toastify";
import {Utility} from "./utility";
import LoadingSpinner from "./components/loading-spinner";
import ModalManager from "./components/modal-manager";
import {ConfigurationManager} from "./configuration-manager";

interface WOAPGlobalsInterface {
    PAGManager: PAGManager;
    ResizeWatcher: Utility.ResizeWatcher;
    MouseWatcher: Utility.MouseWatcher;
    ContextMenuManager: Utility.ContextMenuManager;
    ScrollbarsSize: { horizontalBarHeight: number, verticalBarWidth: number };
    ServiceLocator: ServiceLocator;
}

declare global {
    interface Window {
        WOAP: WOAPGlobalsInterface;
    }
}

interface AppProps {
    history: History
}

interface AppState {
    connected: boolean;
    hostReady: boolean;
}

export default class App extends React.Component<AppProps, AppState> implements MessageReceiver, SystemStoreListener, APICoreConnectionListener {
    public static readonly MessageTypeId = {
        HostError: 0,
        HostReady: 1
    };

    protected ignoreAuthenticationChange: boolean = false;
    protected static lastGeneralErrorTime: number = 0;

    constructor(props) {
        super(props);

        this.state = {
            connected: false,
            hostReady: false
        };
    }

    public componentWillMount(): void {
        let serviceLocator = new ServiceLocator();

        window.WOAP = {
            PAGManager: null,
            ResizeWatcher: new Utility.ResizeWatcher(),
            MouseWatcher: new Utility.MouseWatcher(),
            ContextMenuManager: new Utility.ContextMenuManager(),
            ScrollbarsSize: Utility.getScrollbarsSize(),
            ServiceLocator: serviceLocator
        };

        serviceLocator.setMainComponent(this)
            .setAPIWorker(new APICore("ws://" + window.location.hostname + ":5021"))
            .setStoresManager(new StoresManager())
            .setPAGManager(new PAGManager());

        window.WOAP.PAGManager = serviceLocator.getPAGManager();

        /* No need to listen to this first action */
        serviceLocator.getStoresManager().SystemStore
            .setConnectionStatus(this.state.connected, this.state.hostReady)
            .addListener(this);

        serviceLocator.getAPIWorker()
            .addConnectionListener(this)
            .addListener(ApiCoreReadyMessage.getAddress(), this, App.MessageTypeId.HostReady)
            .addListener(ApiCoreErrorMessage.getAddress(), this, App.MessageTypeId.HostError);

        serviceLocator.getAPIWorker().initializeConnection();

        setTimeout(() => {
            Utility.clearOldLocalStorageValuesWithTimestamp();
        }, 30000);
    }

    public componentWillUnmount(): void {
        window.WOAP.ResizeWatcher.stop();
        window.WOAP.MouseWatcher.stop();

        window.WOAP.ServiceLocator.getStoresManager().SystemStore.removeListener(this);
        window.WOAP.ServiceLocator.getAPIWorker().removeConnectionListener(this);
    }

    public goTo(newPath: string) {
        this.props.history.push(newPath);
    }

    public apiCoreConnected(): void {
        window.WOAP.ServiceLocator.getStoresManager().SystemStore.setConnectionStatus(true, false);
    }

    public apiCoreDisconnected(): void {
        window.WOAP.ServiceLocator.getStoresManager().SystemStore.setConnectionStatus(false, false);
    }

    public connectionStatusChanged(connected: boolean, hostReady: boolean): void {
        this.setState({connected: connected, hostReady: hostReady});
    }

    public oscMessageReceived(message: OSCMessage, messageType: number): void {
        switch (messageType) {
            case App.MessageTypeId.HostError: {
                let msg = new ApiCoreErrorMessage(message);
                if (!msg.isValid()) {
                    return;
                }

                window.WOAP.ServiceLocator.getStoresManager().SystemStore.setConnectionStatus(true, false);
                break;
            }
            case App.MessageTypeId.HostReady: {
                let msg = new ApiCoreReadyMessage(message);

                if (!msg.isValid()) {
                    return;
                }

                window.WOAP.ServiceLocator.getAPIWorker()
                    .request(new ApiCoreGetServerInfoMessage(), ApiCoreServerInfoMessage.getAddress())
                    .then((message) => {
                        let msg = new ApiCoreServerInfoMessage(message);
                        if (!msg.isValid()) {
                            return;
                        }

                        this.ignoreAuthenticationChange = true;

                        window.WOAP.ServiceLocator.getStoresManager().SystemStore
                            .setServerInfo(ServerInfo.fromOSCMessage(msg))
                            .setConnectionStatus(true, true)
                            .setAuthenticated(msg.getUserAuthenticated());

                        this.ignoreAuthenticationChange = false;

                        this.runInitialization();
                    }).catch((err) => App.showGeneralErrorBoxWithDebugMessage("Can\"t fetch server info"));
                break;
            }
            default:
                break;
        }
    }

    public oscBundleReceived(bundle: OSCBundle, messageType: number): void {
    }

    public serverInfoChanged(info: ServerInfo): void {
    }

    public runInitialization(): void {
        if (!window.WOAP.ServiceLocator.getStoresManager().SystemStore.isAuthenticated()) {
            this.goTo("/login");
        } else {
            this.goTo("/");
            window.WOAP.ServiceLocator.getStoresManager().reloadStores();
        }
    }

    public authenticatedStatusChanged(authenticated: boolean): void {
        if (!this.ignoreAuthenticationChange)
            this.runInitialization();
    }

    public logTagChanged(logTag: string): void {
    }

    public static showErrorBox(message: string) {
        toast.error(message, {
            closeButton: false,
            className: "toastBox toastErrorBox"
        });
    }

    public static showAlertBox(message: string) {
        toast.warn(message, {
            closeButton: false,
            className: "toastBox toastAlertBox"
        });
    }

    public static showGeneralErrorBox() {
        let now = new Date().getTime();

        if (now - App.lastGeneralErrorTime > 5000) {
            toast.error("There is some problem with application. Try to refresh the page.", {
                closeButton: false,
                autoClose: 5000,
                className: "toastBox toastErrorBox"
            });
            App.lastGeneralErrorTime = now;
        }
    }

    public static showGeneralErrorBoxWithDebugMessage(message: string) {
        if (ConfigurationManager.isDebug()) {
            console.error(message);
        }

        App.showGeneralErrorBox();
    }

    public render() {
        let rootClassName = "appRoot";
        if (!this.state.hostReady || !this.state.connected) {
            rootClassName += " connecting";
        }

        return (
            <div className={rootClassName}>
                <ModalManager/>

                <Switch>
                    <Route exact path="/" component={Mixer}/>
                    <Route exact path="/login" component={Login}/>
                </Switch>

                <ToastContainer
                    position="top-right"
                    autoClose={5000}
                    hideProgressBar={true}
                    newestOnTop={false}
                    closeOnClick
                    pauseOnHover
                />
                <div className="loadingScreen">
                    <LoadingSpinner size={70}/>
                </div>
            </div>
        );
    }
}