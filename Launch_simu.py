import importlib
import os
import re
import subprocess
import time
import uuid
import sem.utils
import itertools
import csv
import random
import numpy as np
import pandas as pd
from multiprocessing import Pool
from functools import partial
import matplotlib.pyplot as plt
import matplotlib


from tqdm import tqdm

this_path  = os.path.realpath(__file__)
ns3_dir    = os.path.dirname(this_path)
result_dir = ns3_dir + "/Results_Simu"


def launch_simu(script, params, result_Filename, result_Header = None ,runs=10, optimized= False,show_progess= True,get_AllRun=False,max_processes=None):

    script_executable, environment = configure_simu(script= script ,optimized= optimized)
    print("The script for the simulation is : " + str(script_executable))

    params['RngRun'] =np.arange(runs)
    params_Header = list(params.keys())

    if isinstance(result_Header,int) :
        N_results = result_Header
    else:
        N_results= len(result_Header)

    params_reshaped = [np.squeeze(p) if np.ndim(p)!=1 else p  for p in params.values()]
    Combinations = list(itertools.product(*params_reshaped)) #list(Combinations) 
    parameter_Combinations = [(i,)+p for i,p in enumerate(Combinations)]
    random.shuffle(parameter_Combinations)
    
    with Pool(processes=max_processes) as pool:
        errors = 0
        pool_OneSimu = partial(run_OneRun,script_executable = script_executable, environment = environment, parameter_Name = params_Header , len_Results = N_results,runs=runs)
        if show_progess :
            outputs = tqdm(pool.imap_unordered(pool_OneSimu, parameter_Combinations),
                            total=len(parameter_Combinations), 
                            unit='sim.', 
                            desc='Running sim.')
        else : 
            outputs = pool.imap_unordered(pool_OneSimu, parameter_Combinations)

        if get_AllRun:
            Result_buffer = np.zeros((len(parameter_Combinations),N_results))
            for sim_Number, result, errcode in outputs:
                Result_buffer[sim_Number,:] = result 
                
                if errcode != 0 :
                    errors += errcode
                    print("Error detected : simulation result is " +str(result) + "   error code is " + str(errcode))           
                
        else:
            Result_buffer = np.zeros((len(parameter_Combinations)//runs,N_results))
            for sim_Number, result, errcode in outputs:
                Result_buffer[sim_Number//runs,:] += result            
                if errcode != 0 :
                    errors += errcode
                    print("Error detected : simulation result is " +str(result) + "   error code is " + str(errcode))
    
        if errors != 0 :
            print(" N. error detected : " +str(errors) )

        if isinstance(result_Header,int) :
            df = pd.DataFrame(Result_buffer)
        else:
            df = pd.DataFrame(Result_buffer, columns =result_Header)

        df.to_csv(result_dir+"/"+result_Filename+'.csv', index = False)   


    ####################################################
    # Configure simulation environment and parameters  #
    ####################################################
def configure_simu(script,optimized= False):
    if optimized:
        # For old ns-3 installations, the library is in build, while for
        # recent ns-3 installations it's in build/lib. Both paths are
        # thus required to support all versions of ns-3.
        library_path = "%s:%s" % (
            os.path.join(ns3_dir, 'build/optimized'),
            os.path.join(ns3_dir, 'build/optimized/lib'))
    else:
        library_path = "%s:%s" % (
            os.path.join(ns3_dir, 'build/debug'),
            os.path.join(ns3_dir, 'build/debug/lib'))

    # We use both LD_ and DYLD_ to support Linux and Mac OS.
    environment = {
        'LD_LIBRARY_PATH': library_path,
        'DYLD_LIBRARY_PATH': library_path}

    # ns-3's build status output is used to get the executable path for the
    # specified script.
    if optimized:
        build_status_path = os.path.join(ns3_dir,
                                            'build/optimized/build-status.py')
    else:
        build_status_path = os.path.join(ns3_dir,
                                            'build/debug/build-status.py')

    # By importing the file, we can naturally get the dictionary
    try:  # This only works on Python >= 3.5
        spec = importlib.util.spec_from_file_location('build_status',
                                                        build_status_path)
        build_status = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(build_status)
    except (AttributeError):  # This happens in Python <= 3.4
        import imp
        build_status = imp.load_source('build_status', build_status_path)
    
    # Search is simple: we look for the script name in the program field.
    # Note that this could yield multiple matches, in case the script name
    # string is contained in another script's name.
    # matches contains [program, path] for each program matching the script
    matches = [{'name': program,
                'path': os.path.abspath(os.path.join(ns3_dir, program))} for
                program in build_status.ns3_runnable_programs if script
                in program]

    if not matches:
        raise ValueError("Cannot find %s script" % script)

    # To handle multiple matches, we take the 'better matching' option,
    # i.e., the one with length closest to the original script name.
    match_percentages = map(lambda x: {'name': x['name'],
                                        'path': x['path'],
                                        'percentage':
                                        len(script)/len(x['name'])},
                            matches)

    script_executable = max(match_percentages,
                                    key=lambda x: x['percentage'])['path']
    
    return script_executable, environment



    ####################################################
    # Run multiple simulations given a parameter list  #
    ####################################################
def run_simu(script_executable, environment,Result_buffer, writer, parameter_List, parameter_Name,len_Results,runs=10):
    errors = 0
    sim_Number,  *params = parameter
    for parameter in tqdm(parameter_List, total=len(parameter_List), unit='sim.', desc='Running sim.') :

        command = [script_executable] + ['--%s=%s' % (param, value)
                                                for param, value in
                                                zip(parameter_Name,parameter)]

        start = time.time()  # Time execution
        res = np.zeros(len_Results,dtype = float)
        errcode = 0
        for i in range(runs):
            proc = subprocess.Popen(command, cwd=result_dir,
                                            env=environment,
                                            stdout=subprocess.PIPE,
                                            stderr=subprocess.PIPE)
            
            proc.wait()
            stdout, stderr = proc.communicate()
            errors += proc.returncode
            stdout = list(stdout.decode().split())
            results = [float(x) for x in stdout]
            res += results

        results_ToWrite = list(res/runs)
        end = time.time()  # Time execution
        Result_buffer[sim_Number,:] = results_ToWrite 
        #print("Run time : " + str(round(end-start,2)))

    return Result_buffer, errors

    ####################################################
    # Run one simulation given a parameter combination #
    ####################################################
def run_OneRun(parameter , script_executable, environment, parameter_Name,len_Results,runs=10):
    sim_Number,  *params = parameter
    command = [script_executable] + ['--%s=%s' % (param, value)
                                            for param, value in
                                            zip(parameter_Name,params)]

    start = time.time()  # Time execution
    proc = subprocess.Popen(command, cwd=result_dir,
                                    env=environment,
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
    
    proc.wait()
    stdout, stderr = proc.communicate()
    errcode = proc.returncode
    stdout = list(stdout.decode().split())
    results = [float(x) for x in stdout]
    end = time.time()  # Time execution
    #print("Run time : " + str(round(end-start,2)))
    return sim_Number, results , errcode



def get_Results(params , result_Filename,get_AllRun):
    Nparams = len(params) +1
    params_values = list(params.values())
    shape_Res = [np.size(p)  for p  in params_values]
    if get_AllRun == False:
        shape_Res = shape_Res[:-1]

    Results_df = pd.read_csv(result_dir+"/"+result_Filename+".csv")
    Results_np = Results_df.to_numpy()
    shape_Res = shape_Res + [len(Results_np[0])]
    Results_re = np.reshape(Results_np,shape_Res)

    return params_values, Results_re


if __name__ == '__main__':
    """params = {
        'nDevices': [300],
        'simulationTime': round(i, 4) for i in np.arange(10, 600, step= 50, dtype='float')],
        'radius': [0, 7500],
        'interferenceMatrix': ["aloha"] #lambda p: ["aloha"] if p["radius"] == 0 else ["goursaud"],
    }"""
    params = {
        'nDevices': [300],
        'simulationTime':  [round(i, 4) for i in np.logspace(0.5, 4, base=60, num=10, dtype='float')],
        'radius': [0, 7500],
        'interferenceMatrix': ["aloha"] #lambda p: ["aloha"] if p["radius"] == 0 else ["goursaud"],
    }

    runs = 5
    get_AllRun=False
    Result_FileName = 'Test'
    #myTestLoRaWAN
    launch_simu('aloha-throughput', 
                params, 
                Result_FileName, 
                ["SF7_Gen", "SF7_Rec","SF8_Gen", "SF8_Rec","SF9_Gen", "SF9_Rec","SF10_Gen", "SF10_Rec","SF11_Gen", "SF11_Rec","SF12_Gen", "SF12_Rec"],
                runs=runs, 
                optimized= True,
                show_progess=False,
                get_AllRun=get_AllRun,
                max_processes = None)

    get_Results(params,Result_FileName,get_AllRun=get_AllRun)



