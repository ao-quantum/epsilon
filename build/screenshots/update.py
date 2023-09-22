import sys, os, argparse
import helper
import args_types
from print_format import print_bold

parser = argparse.ArgumentParser(description='This script updates the screenshot and the crc32 of a state file in the test screenshots dataset.')
parser.add_argument('executable', metavar='EXE', type=args_types.existing_file, help='epsilon executable')
parser.add_argument('name', metavar='NAME', help='name of scenario folder in the test screenshots dataset')

def main():
   # Parse args
   args = parser.parse_args()

   # Print warning
   print_bold("Your are updating screenshots and crc32 for this scenario.")
   print_bold("Please make sure before launching this script that you checked screenshots AT EACH STEP and with THE SAME executable.")
   print_bold("If you have a doubt, run take_for_each_step.py with the same executable.")
   print("-------")

   scenario_folder = helper.folder(args.name)
   if not os.path.isdir(scenario_folder):
      print("Error:", args.name, "is not a folder in the test screenshots dataset")
      sys.exit(1)
   state_file = helper.get_file_with_extension(scenario_folder, ".nws")
   reference_image =  helper.get_file_with_extension(scenario_folder, ".png")
   crc_file = helper.get_file_with_extension(scenario_folder, ".txt")
   if state_file == '' or reference_image == '' or crc_file == '':
      print("Error:", scenario_folder, "should contain 1 .nws, 1 .png and 1 .txt")
      sys.exit(1)

   # Update crc32
   print("Rewriting", crc_file)
   helper.compute_and_store_crc32(state_file, args.executable, crc_file)

if __name__ == "__main__":
    main()
