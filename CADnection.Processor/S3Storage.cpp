#include "S3Storage.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <boost/filesystem.hpp>

//bool S3Storage::CreateBucketIfAny(const Aws::String& bucketName, const Aws::S3::Model::BucketLocationConstraint& region) {
	//Aws::S3::S3Client s3Client;
	//try {
		/*Aws::S3::Model::HeadBucketRequest hbr;
		hbr.SetBucket(bucketName);
		Aws::S3::Model::HeadBucketOutcome ret = s3Client.HeadBucket(hbr);
*/
	//	return true;
	//}
	//catch (const std::exception& ex) {
		// Set up the request
		//Aws::S3::Model::CreateBucketRequest request;
		//request.SetBucket(bucketName);
		//// Is the region other than us-west-1 (N. California)?
		//if (region != REGION_DEFAULT) {
		//	// Specify the region as a location constraint
		//	Aws::S3::Model::CreateBucketConfiguration bucketConfig;
		//	bucketConfig.SetLocationConstraint(region);
		//	request.SetCreateBucketConfiguration(bucketConfig);
		//}
		//// Create the bucket
		//auto outcome = s3Client.CreateBucket(request);
		//if (!outcome.IsSuccess()) {
		//	auto err = outcome.GetError();
		//	std::cout << "ERROR: CreateBucket: " << err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
		//	return false;
		//}

//		return true;
//	}
//}

//bool S3Storage::PutObject(const Aws::String& bucketName, const Aws::String& s3ObjectName, const char* filename, const Aws::String& region) {
	// Verify file_name exists
	//if (!boost::filesystem::exists(filename)) {
	//	std::cout << "ERROR: NoSuchFile: The specified file " << filename << " does not exist." << std::endl;
	//	return false;
	//}

	//Aws::Client::ClientConfiguration clientConfig;
	//if (!region.empty()) {
	//	clientConfig.region = region;
	//}

	//Aws::S3::S3Client s3Client(clientConfig);
	//Aws::S3::Model::PutObjectRequest objectRequest;
	//objectRequest.SetBucket(bucketName);
	//objectRequest.SetKey(s3ObjectName);
	//const std::shared_ptr<Aws::IOStream> inputData = Aws::MakeShared<Aws::FStream>("cadnection", filename, std::ios_base::in | std::ios_base::binary);
	//objectRequest.SetBody(inputData);
	//auto ret = s3Client.PutObject(objectRequest);
	//if (!ret.IsSuccess()) {
	//	auto error = ret.GetError();
	//	std::cout << "ERROR: " << error.GetExceptionName() << ": " << error.GetMessage() << std::endl;
	//	return false;
	//}
//
//	return true;
//}
