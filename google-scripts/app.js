function showURL() {
  var cpService = getCloudPrintService();
  if (!cpService.hasAccess()) {
    Logger.log(cpService.getAuthorizationUrl());
  }
}

function getCloudPrintService() {
  return OAuth2.createService('print')
    .setAuthorizationBaseUrl('https://accounts.google.com/o/oauth2/auth')
    .setTokenUrl('https://accounts.google.com/o/oauth2/token')
    .setClientId('### Client ID ##')
    .setClientSecret('### Client secret ###')
    .setCallbackFunction('authCallback')
    .setPropertyStore(PropertiesService.getUserProperties())
    .setScope('https://www.googleapis.com/auth/cloudprint')
    .setParam('login_hint', Session.getActiveUser().getEmail())
    .setParam('access_type', 'offline')
    .setParam('approval_prompt', 'force');
}

function authCallback(request) {
  var isAuthorized = getCloudPrintService().handleCallback(request);
  if (isAuthorized) {
    return HtmlService.createHtmlOutput('You can now use Google Cloud Print from Apps Script.');
  } else {
    return HtmlService.createHtmlOutput('Cloud Print Error: Access Denied');
  }
}

function printGoogleDocument(docID, printerID, docName) {
  var ticket = {
    version: "1.0",
    print: {
      color: {
        type: "STANDARD_COLOR",
        vendor_id: "Color"
      },
      duplex: {
        type: "NO_DUPLEX"
      }
    }
  };

  var payload = {
    "printerid" : printerID,
    "title"     : docName,
    "content"   : DriveApp.getFileById(docID).getBlob(),
    "contentType": "application/pdf",
    "ticket"    : JSON.stringify(ticket)
  };

  var response = UrlFetchApp.fetch('https://www.google.com/cloudprint/submit', {
    method: "POST",
    payload: payload,
    headers: {
      Authorization: 'Bearer ' + getCloudPrintService().getAccessToken()
    },
    "muteHttpExceptions": true
  });

  response = JSON.parse(response);

  if (response.success) {
    Logger.log("%s", response.message);
  } else {
    Logger.log("Error Code: %s %s", response.errorCode, response.message);
  }
}

function print() {
  printGoogleDocument('### Doc ID ###', '### Printer ID ###', 'Monitorizare');
  clearOldData();
}

function clearOldData(){
  SpreadsheetApp.getActive().getSheetByName('Camera frigorifica').getRange('A3:D15').clearContent();
  SpreadsheetApp.getActive().getSheetByName('Materie prima').getRange('A3:D15').clearContent();
  SpreadsheetApp.getActive().getSheetByName('Frigider hamei').getRange('A3:D15').clearContent();
  SpreadsheetApp.getActive().getSheetByName('Frigider drojdie').getRange('A3:D15').clearContent();
  SpreadsheetApp.getActive().getSheetByName('Ventilatie').getRange('A3:D100').clearContent();
}