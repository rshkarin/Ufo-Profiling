# Include the Dropbox SDK

import sys
import argparse
import dropbox
import webbrowser
import os
from dropbox import client, rest, session
from oauth2client import file
from oauth2client import client
from oauth2client import tools

# Get your app key and secret from the Dropbox developer website
app_key = '07vo72tow65pgyw'
app_secret = '6nmcjebfok1kvrh'
access_token_file = '.access_token_info'

def main(argv):
  # Parse the command-line flags.
  parser = argparse.ArgumentParser(description='Dropbox uploader for Ufo-profiler')
  parser.add_argument('-i', '--input_file', required=True, type=str, help='path to the file')
  parser.add_argument('-o', '--output_path', required=True, type=str, help='path to the output path')

  try:
    args = parser.parse_args()
  except argparse.ArgumentTypeError as err:
    print err
    sys.exit(1)

  flow = dropbox.client.DropboxOAuth2FlowNoRedirect(app_key, app_secret)
  
  access_token = None
  if os.path.isfile(access_token_file):
    f = open(access_token_file, 'r')
    access_token = f.read()
    f.close()

  if not access_token:
    # Have the user sign in and authorize this token
    authorize_url = flow.start()
    print '1. Go to: ' + authorize_url
    webbrowser.open(authorize_url, new=2)
    print '2. Click "Allow" (you might have to log in first)'
    print '3. Copy the authorization code.'
    code = raw_input("Enter the authorization code here: ").strip()

    # This will fail if the user enters an invalid authorization code
    access_token, user_id = flow.finish(code)
    if access_token:
      try:
        f = open(access_token_file, 'w')
      except IOError as err:
        print "Can't write to file {0}!".format(access_token_file)
        sys.exit(1)
      
      f.write(access_token)
      f.close()

  client = dropbox.client.DropboxClient(access_token)
  
  input_file = args.input_file
  target_path = args.output_path
  
  base_filename = os.path.splitext(os.path.basename(input_file))[0];
  target_path_file = os.path.join(target_path, base_filename)

  f = open(input_file, 'rb')
  response = client.put_file(target_path_file, f)
  print "File {0} has been inserted!".format(base_filename)

if __name__ == '__main__':
  main(sys.argv)
