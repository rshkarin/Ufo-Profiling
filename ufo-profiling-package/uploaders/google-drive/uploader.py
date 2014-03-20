# Copyright (C) 2013 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import httplib2
import os
import sys

from apiclient import discovery
from apiclient.discovery import build
from apiclient.http import MediaFileUpload
from oauth2client import file
from oauth2client import client
from oauth2client import tools
from os.path import basename
from os.path import splitext

# Parser for command-line arguments.
parser = argparse.ArgumentParser(
    description=__doc__,
    formatter_class=argparse.RawDescriptionHelpFormatter,
    parents=[tools.argparser])
parser.add_argument('-i', '--input_file_path', required=True, type=str, help='path to the file of input data')
parser.add_argument('-o', '--output_file_path', required=True, type=str, help='path to the file of output data')


# CLIENT_SECRETS is name of a file containing the OAuth 2.0 information for this
# application, including client_id and client_secret. You can see the Client ID
# and Client secret on the APIs page in the Cloud Console:
# <https://cloud.google.com/console#/project/643231083785/apiui>
CLIENT_SECRETS = os.path.join(os.path.dirname(__file__), 'client_secrets.json')

# Set up a Flow object to be used for authentication.
# Add one or more of the following scopes. PLEASE ONLY ADD THE SCOPES YOU
# NEED. For more information on using scopes please see
# <https://developers.google.com/+/best-practices>.
FLOW = client.flow_from_clientsecrets(CLIENT_SECRETS,
  scope=[
      'https://www.googleapis.com/auth/drive',
      'https://www.googleapis.com/auth/drive.appdata',
      'https://www.googleapis.com/auth/drive.apps.readonly',
      'https://www.googleapis.com/auth/drive.file',
      'https://www.googleapis.com/auth/drive.metadata.readonly',
      'https://www.googleapis.com/auth/drive.readonly',
      'https://www.googleapis.com/auth/drive.scripts',
    ],
    message=tools.message_if_missing(CLIENT_SECRETS))

def insert_file(service, title, description, parent_id, mime_type, filename):
  media_body = MediaFileUpload(filename, mimetype=mime_type, resumable=True)
  body = {
    'title': title,
    'description': description,
    'mimeType': mime_type
  }
  # Set the parent folder.
  if parent_id:
    body['parents'] = [{'id': parent_id}]

  try:
    file = service.files().insert(body=body, media_body=media_body).execute()

    return file
  except errors.HttpError, error:
    print 'An error occured: %s' % error
    return None

def insert_folder(service, title, description, parent_id):
  mime_type = 'application/vnd.google-apps.folder'
  body = {
    'title': title,
    'description': description,
    'mimeType': mime_type
  }

  # Set the parent folder.
  if parent_id:
    body['parents'] = [{'id': parent_id}]

  try:
    file = service.files().insert(body=body).execute()
    return file
  except errors.HttpError, error:
    print 'An error occured: %s' % error
    return None


def main(argv):
  # Parse the command-line flags.
  flags = parser.parse_args(argv[1:])

  # If the credentials don't exist or are invalid run through the native client
  # flow. The Storage object will ensure that if successful the good
  # credentials will get written back to the file.
  storage = file.Storage('credentials.dat')
  credentials = storage.get()
  if credentials is None or credentials.invalid:
    credentials = tools.run_flow(FLOW, storage, flags)

  # Create an httplib2.Http object to handle our HTTP requests and authorize it
  # with our good Credentials.
  http = httplib2.Http()
  http = credentials.authorize(http)

  # Construct the service object for the interacting with the Drive API.
  service = discovery.build('drive', 'v2', http=http)

  try:
    input_file = flags.input_file_path
    target_path = flags.output_file_path
    
    folders = target_path.split('/')
    parent = 'root'

    for folder in folders:
      query = "mimeType = 'application/vnd.google-apps.folder' and title = '{0}' and '{1}' in parents".format(folder, parent)
      files = service.files().list(q=query).execute()['items']

      if len(files):
        parent = files[0]['id']
      else:
        parent = insert_folder(service, folder, 'something', parent)['id']
        print "Folder {0}, has been added!".format(folder)
    
    base_filename, file_extension = splitext(basename(input_file))
    if file_extension == '.svg':    
      f = insert_file(service, base_filename, "Graph of performance", parent, "image/svg+xml", input_file)
    else:
      f = insert_file(service, base_filename, "Measurement of performance on GPU", parent, "text/plain", input_file)

    print "File {0} has been inserted!".format(base_filename)

  except client.AccessTokenRefreshError:
    print ("The credentials have been revoked or expired, please re-run"
      "the application to re-authorize")

if __name__ == '__main__':
  main(sys.argv)
