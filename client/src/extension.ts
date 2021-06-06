/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */
import * as path from 'path';
import * as os from 'os';
import {ExtensionContext, workspace} from 'vscode';

import {Executable, LanguageClient, LanguageClientOptions, ServerOptions} from 'vscode-languageclient/node';

let client: LanguageClient;

export function activate(context: ExtensionContext) {

	let serverStartupCommand = os.platform() === 'linux' ?
		 path.join(__dirname, '..', '..', 'server', 'out', 'gw_basic_server')
		 : path.join(__dirname, '..', '..', 'server', 'outw', 'gw_basic_server.exe');
	console.log(serverStartupCommand);
	let serverExe: Executable = {
		command: serverStartupCommand,	
		args: ['config_file=' + path.join(__dirname, '..', '..', 'data', 'config.json'),
				'log_file='+path.join(__dirname, '..', '..', 'log')],
		options: {
			env: process.env,
			shell: true
		}
	}

	let serverOptions: ServerOptions = {
		run:  serverExe,
		debug: serverExe
	}

	let clientOptions: LanguageClientOptions = {
		
		documentSelector: [{pattern: '**/*.{bas,gwbas,BAS,GWBAS}'}],
		synchronize: {
			fileEvents: workspace.createFileSystemWatcher('**/.clientrc')
		}
	};

	client = new LanguageClient(
		'gw_basic_server',
		'gw_basic_server',
		serverOptions,
		clientOptions
	);

	client.start();
}

